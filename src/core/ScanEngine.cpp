// Copyright (c) Famatech Corp. All rights reserved.
// Licensed under the proprietary license. See LICENSE for details.

#include "core/ScanEngine.h"
#include "core/ScanWorker.h"
#include "core/DeviceStore.h"
#include "net/IcmpProbe.h"
#include "net/ArpProbe.h"
#include "net/PortScanner.h"
#include "net/DnsResolver.h"
#include "net/NetbiosQuery.h"

#include <QThread>
#include <QtConcurrent>
#include <algorithm>
#include <cmath>

namespace aips::core {

ScanEngine::ScanEngine(QObject* parent)
    : QObject(parent)
    , m_icmpProbe(std::make_unique<net::IcmpProbe>())
    , m_arpProbe(std::make_unique<net::ArpProbe>())
    , m_portScanner(std::make_unique<net::PortScanner>())
    , m_dnsResolver(std::make_unique<net::DnsResolver>())
    , m_netbiosQuery(std::make_unique<net::NetbiosQuery>())
{
    m_threadPool.setMaxThreadCount(optimalThreadCount());
}

ScanEngine::~ScanEngine()
{
    if (m_state.load() != ScanState::Idle)
        cancelScan();

    m_threadPool.waitForDone(5000);
}

void ScanEngine::setScanRange(const ScanRange& range)
{
    if (m_state.load() != ScanState::Idle) {
        emit scanError("Cannot change scan range while scanning");
        return;
    }
    m_range = range;
}

void ScanEngine::setScanOptions(const ScanOptions& options)
{
    if (m_state.load() != ScanState::Idle) {
        emit scanError("Cannot change scan options while scanning");
        return;
    }
    m_options = options;

    int threads = options.threadCount > 0
                  ? options.threadCount
                  : optimalThreadCount();
    m_threadPool.setMaxThreadCount(threads);
}

void ScanEngine::setDeviceStore(std::shared_ptr<DeviceStore> store)
{
    m_deviceStore = std::move(store);
}

int ScanEngine::totalAddresses() const
{
    return m_totalCount;
}

int ScanEngine::scannedAddresses() const
{
    return m_scannedCount.load();
}

int ScanEngine::devicesFound() const
{
    return m_foundCount.load();
}

qint64 ScanEngine::elapsedMs() const
{
    return m_elapsedTimer.isValid() ? m_elapsedTimer.elapsed() : 0;
}

void ScanEngine::startScan()
{
    if (m_state.load() != ScanState::Idle) {
        emit scanError("Scan already in progress");
        return;
    }

    if (!m_range.isValid()) {
        emit scanError("Invalid scan range");
        return;
    }

    setState(ScanState::Preparing);
    m_scannedCount = 0;
    m_foundCount   = 0;
    m_progress     = 0;

    if (m_deviceStore)
        m_deviceStore->clear();

    m_elapsedTimer.start();

    prepareWorkers();
    distributeAddresses();

    setState(ScanState::Scanning);
    emit scanStarted();

    int batchSize = std::min(static_cast<int>(m_workers.size()),
                             m_threadPool.maxThreadCount());
    startWorkerBatch(batchSize);
}

void ScanEngine::pauseScan()
{
    if (m_state.load() != ScanState::Scanning)
        return;

    setState(ScanState::Paused);
    for (auto* worker : m_workers)
        worker->requestCancel();
}

void ScanEngine::resumeScan()
{
    if (m_state.load() != ScanState::Paused)
        return;

    setState(ScanState::Scanning);
    prepareWorkers();
    distributeAddresses();
    startWorkerBatch(m_threadPool.maxThreadCount());
}

void ScanEngine::cancelScan()
{
    if (m_state.load() == ScanState::Idle)
        return;

    setState(ScanState::Cancelling);
    for (auto* worker : m_workers)
        worker->requestCancel();

    m_threadPool.waitForDone(3000);
    m_workers.clear();

    setState(ScanState::Idle);
    emit scanCancelled();
}

void ScanEngine::onWorkerDeviceFound(const Device& device)
{
    QMutexLocker lock(&m_mutex);

    m_foundCount.fetch_add(1);

    if (m_deviceStore) {
        auto existing = m_deviceStore->findByIp(device.ipAddress());
        if (existing.has_value()) {
            Device merged = existing.value();
            mergeDeviceInfo(merged, device);
            m_deviceStore->updateDevice(merged);
            emit deviceUpdated(merged);
        } else {
            m_deviceStore->addDevice(device);
            emit deviceDiscovered(device);
        }
    } else {
        emit deviceDiscovered(device);
    }
}

void ScanEngine::onWorkerFinished(int workerId)
{
    QMutexLocker lock(&m_mutex);

    m_workers.erase(
        std::remove_if(m_workers.begin(), m_workers.end(),
                        [workerId](const ScanWorker* w) { return w->workerId() == workerId; }),
        m_workers.end()
    );

    int scanned = 0;
    for (const auto* w : m_workers)
        scanned += w->scanned();
    m_scannedCount = scanned + m_scannedCount.load();

    int pct = m_totalCount > 0
              ? static_cast<int>(std::round(100.0 * m_scannedCount / m_totalCount))
              : 100;
    m_progress = std::min(pct, 100);
    emit progressChanged(m_progress);

    if (m_workers.isEmpty()) {
        if (m_options.resolveHostnames)
            resolveHostnames();

        if (m_options.enumerateShares)
            enumerateShares();

        finalizeScan();
    }
}

void ScanEngine::onWorkerError(int workerId, const QString& message)
{
    emit scanError(QString("Worker %1: %2").arg(workerId).arg(message));
}

void ScanEngine::setState(ScanState s)
{
    m_state.store(s);
    emit stateChanged(s);
}

void ScanEngine::prepareWorkers()
{
    qDeleteAll(m_workers);
    m_workers.clear();
    m_workerAddresses.clear();
}

void ScanEngine::distributeAddresses()
{
    QVector<QHostAddress> allAddresses = expandRange(m_range);
    m_totalCount = allAddresses.size();

    if (m_totalCount == 0)
        return;

    int threadCount = m_threadPool.maxThreadCount();
    int chunkSize   = std::max(1, m_totalCount / threadCount);

    m_workerAddresses.resize(threadCount);

    for (int i = 0; i < m_totalCount; ++i) {
        int bucket = std::min(i / chunkSize, threadCount - 1);
        m_workerAddresses[bucket].append(allAddresses[i]);
    }

    for (int i = 0; i < threadCount; ++i) {
        if (m_workerAddresses[i].isEmpty())
            continue;

        auto* worker = new ScanWorker(i, m_workerAddresses[i], m_options);
        worker->setAutoDelete(false);

        connect(worker, &ScanWorker::deviceFound,
                this,   &ScanEngine::onWorkerDeviceFound, Qt::QueuedConnection);
        connect(worker, &ScanWorker::finished,
                this,   &ScanEngine::onWorkerFinished, Qt::QueuedConnection);
        connect(worker, &ScanWorker::error,
                this,   &ScanEngine::onWorkerError, Qt::QueuedConnection);

        m_workers.append(worker);
    }
}

void ScanEngine::startWorkerBatch(int batchSize)
{
    int count = std::min(batchSize, static_cast<int>(m_workers.size()));
    for (int i = 0; i < count; ++i)
        m_threadPool.start(m_workers[i]);
}

void ScanEngine::resolveHostnames()
{
    setState(ScanState::Resolving);

    if (!m_deviceStore)
        return;

    auto devices = m_deviceStore->allDevices();
    for (auto& device : devices) {
        if (m_state.load() == ScanState::Cancelling)
            return;

        if (device.hostname().isEmpty()) {
            QString hostname = m_dnsResolver->resolve(device.ipAddress(), m_options.dnsTimeoutMs);
            if (!hostname.isEmpty()) {
                device.setHostname(hostname);
                m_deviceStore->updateDevice(device);
                emit deviceUpdated(device);
            }
        }

        if (m_options.resolveNetbios && device.netbiosName().isEmpty()) {
            auto info = m_netbiosQuery->query(device.ipAddress(), m_options.netbiosTimeoutMs);
            if (info.has_value()) {
                device.setNetbiosName(info->name);
                device.setWorkgroup(info->workgroup);
                m_deviceStore->updateDevice(device);
                emit deviceUpdated(device);
            }
        }
    }
}

void ScanEngine::enumerateShares()
{
    setState(ScanState::EnumeratingShares);

    if (!m_deviceStore)
        return;

    auto devices = m_deviceStore->allDevices();
    for (auto& device : devices) {
        if (m_state.load() == ScanState::Cancelling)
            return;

        device.clearSharedResources();

        // SMB shares
        auto smbShares = m_netbiosQuery->enumerateShares(device.ipAddress());
        for (const auto& share : smbShares) {
            SharedResource res;
            res.protocol    = SharedResource::Protocol::SMB;
            res.path        = share.path;
            res.displayName = share.name;
            res.port        = 445;
            res.requiresAuth = share.requiresAuth;
            device.addSharedResource(res);
        }

        // HTTP/HTTPS probing
        if (m_options.checkHttpPorts) {
            for (uint16_t port : device.openPorts()) {
                if (port == 80 || port == 8080) {
                    SharedResource res;
                    res.protocol    = SharedResource::Protocol::HTTP;
                    res.path        = QString("http://%1:%2/").arg(device.ipAddress().toString()).arg(port);
                    res.displayName = QString("HTTP :%1").arg(port);
                    res.port        = port;
                    res.requiresAuth = false;
                    device.addSharedResource(res);
                }
                if (port == 443 || port == 8443) {
                    SharedResource res;
                    res.protocol    = SharedResource::Protocol::HTTPS;
                    res.path        = QString("https://%1:%2/").arg(device.ipAddress().toString()).arg(port);
                    res.displayName = QString("HTTPS :%1").arg(port);
                    res.port        = port;
                    res.requiresAuth = false;
                    device.addSharedResource(res);
                }
            }
        }

        // FTP probing
        if (m_options.checkFtpPorts) {
            for (uint16_t port : device.openPorts()) {
                if (port == 21) {
                    SharedResource res;
                    res.protocol    = SharedResource::Protocol::FTP;
                    res.path        = QString("ftp://%1/").arg(device.ipAddress().toString());
                    res.displayName = QStringLiteral("FTP");
                    res.port        = 21;
                    res.requiresAuth = false;
                    device.addSharedResource(res);
                }
            }
        }

        m_deviceStore->updateDevice(device);
        emit deviceUpdated(device);
    }
}

void ScanEngine::finalizeScan()
{
    setState(ScanState::Finalizing);

    qint64 elapsed = m_elapsedTimer.elapsed();

    setState(ScanState::Idle);
    emit scanFinished(m_foundCount.load(), elapsed);
}

void ScanEngine::mergeDeviceInfo(Device& target, const Device& source)
{
    if (target.hostname().isEmpty() && !source.hostname().isEmpty())
        target.setHostname(source.hostname());

    if (target.vendor().isEmpty() && !source.vendor().isEmpty())
        target.setVendor(source.vendor());

    if (target.osFamily().isEmpty() && !source.osFamily().isEmpty())
        target.setOsFamily(source.osFamily());

    if (target.netbiosName().isEmpty() && !source.netbiosName().isEmpty())
        target.setNetbiosName(source.netbiosName());

    if (source.status() == DeviceStatus::Online)
        target.setStatus(DeviceStatus::Online);

    if (source.latencyMs() > 0 && (target.latencyMs() == 0 || source.latencyMs() < target.latencyMs()))
        target.setLatencyMs(source.latencyMs());

    target.setLastSeen(QDateTime::currentDateTime());

    for (uint16_t port : source.openPorts()) {
        if (!target.openPorts().contains(port))
            target.addOpenPort(port);
    }
}

QVector<QHostAddress> ScanEngine::expandRange(const ScanRange& range) const
{
    return range.expandAll();
}

int ScanEngine::optimalThreadCount() const
{
    int cores = QThread::idealThreadCount();
    return std::clamp(cores * 4, 8, 256);
}

} // namespace aips::core
