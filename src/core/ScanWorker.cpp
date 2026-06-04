// Copyright (c) Famatech Corp. All rights reserved.
// Licensed under the proprietary license. See LICENSE for details.

#include "core/ScanWorker.h"
#include "net/IcmpProbe.h"
#include "net/ArpProbe.h"

#include <QThread>
#include <QElapsedTimer>

#ifdef Q_OS_WIN
#include <winsock2.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")
#endif

namespace aips::core {

ScanWorker::ScanWorker(int workerId,
                       const QVector<QHostAddress>& addresses,
                       const ScanOptions& options,
                       QObject* parent)
    : QObject(parent)
    , m_workerId(workerId)
    , m_addresses(addresses)
    , m_options(options)
{
    setAutoDelete(false);
}

ScanWorker::~ScanWorker() = default;

void ScanWorker::run()
{
    const int total = m_addresses.size();

    for (int i = 0; i < total; ++i) {
        if (m_cancelled.load())
            break;

        try {
            Device device = probeAddress(m_addresses[i]);

            if (device.status() == DeviceStatus::Online ||
                !m_options.skipOfflineDevices) {
                m_found.fetch_add(1);
                emit deviceFound(device);
            }
        } catch (const std::exception& e) {
            emit error(m_workerId, QString("Error probing %1: %2")
                       .arg(m_addresses[i].toString(), e.what()));
        }

        m_scanned.fetch_add(1);

        if (i % 10 == 0)
            emit progressUpdate(m_workerId, m_scanned.load(), total);

        throttle();
    }

    emit finished(m_workerId);
}

void ScanWorker::requestCancel()
{
    m_cancelled.store(true);
}

Device ScanWorker::probeAddress(const QHostAddress& addr)
{
    Device device(addr);
    device.setLastSeen(QDateTime::currentDateTime());

    bool alive = false;

    if (m_options.useIcmp) {
        alive = pingHost(addr, m_options.icmpTimeoutMs, m_options.icmpRetries);
    }

    MacAddress mac{};
    if (m_options.useArp && arpResolve(addr, mac)) {
        device.setMacAddress(mac);
        if (!alive)
            alive = true;
    }

    device.setStatus(alive ? DeviceStatus::Online : DeviceStatus::Offline);

    return device;
}

bool ScanWorker::pingHost(const QHostAddress& addr, int timeoutMs, int retries)
{
#ifdef Q_OS_WIN
    HANDLE hIcmp = IcmpCreateFile();
    if (hIcmp == INVALID_HANDLE_VALUE)
        return false;

    char sendData[32] = "AdvancedIPScanner-Ping";
    DWORD replySize = sizeof(ICMP_ECHO_REPLY) + sizeof(sendData) + 8;
    auto replyBuffer = std::make_unique<BYTE[]>(replySize);

    for (int attempt = 0; attempt < retries; ++attempt) {
        if (m_cancelled.load()) {
            IcmpCloseHandle(hIcmp);
            return false;
        }

        DWORD ret = IcmpSendEcho(
            hIcmp,
            htonl(addr.toIPv4Address()),
            sendData,
            sizeof(sendData),
            nullptr,
            replyBuffer.get(),
            replySize,
            static_cast<DWORD>(timeoutMs)
        );

        if (ret > 0) {
            auto* reply = reinterpret_cast<PICMP_ECHO_REPLY>(replyBuffer.get());
            if (reply->Status == IP_SUCCESS) {
                IcmpCloseHandle(hIcmp);
                return true;
            }
        }
    }

    IcmpCloseHandle(hIcmp);
    return false;
#else
    Q_UNUSED(addr);
    Q_UNUSED(timeoutMs);
    Q_UNUSED(retries);
    return false;
#endif
}

bool ScanWorker::arpResolve(const QHostAddress& addr, MacAddress& outMac)
{
#ifdef Q_OS_WIN
    ULONG macBuf[2];
    ULONG macLen = 6;
    DWORD ret = SendARP(
        htonl(addr.toIPv4Address()),
        0,
        macBuf,
        &macLen
    );

    if (ret == NO_ERROR && macLen == 6) {
        auto* bytes = reinterpret_cast<uint8_t*>(macBuf);
        std::copy(bytes, bytes + 6, outMac.begin());
        return true;
    }
    return false;
#else
    Q_UNUSED(addr);
    Q_UNUSED(outMac);
    return false;
#endif
}

void ScanWorker::throttle() const
{
    if (m_options.scanSpeedPercent < 100) {
        int delayUs = static_cast<int>(
            (100 - m_options.scanSpeedPercent) * 50
        );
        QThread::usleep(delayUs);
    }
}

} // namespace aips::core
