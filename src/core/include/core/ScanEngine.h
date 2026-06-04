// Copyright (c) Famatech Corp. All rights reserved.
// Licensed under the proprietary license. See LICENSE for details.

#pragma once

#include "core/Device.h"
#include "core/ScanRange.h"
#include "core/ScanOptions.h"

#include <QObject>
#include <QHostAddress>
#include <QThreadPool>
#include <QMutex>
#include <QElapsedTimer>

#include <memory>
#include <atomic>
#include <functional>

namespace aips::net {
    class IcmpProbe;
    class ArpProbe;
    class PortScanner;
    class DnsResolver;
    class NetbiosQuery;
}

namespace aips::core {

class ScanWorker;
class DeviceStore;

class ScanEngine : public QObject {
    Q_OBJECT
    Q_PROPERTY(ScanState state READ state NOTIFY stateChanged)
    Q_PROPERTY(int progress READ progress NOTIFY progressChanged)

public:
    enum class ScanState : uint8_t {
        Idle,
        Preparing,
        Scanning,
        Resolving,
        EnumeratingShares,
        Finalizing,
        Paused,
        Cancelling
    };
    Q_ENUM(ScanState)

    explicit ScanEngine(QObject* parent = nullptr);
    ~ScanEngine() override;

    ScanEngine(const ScanEngine&) = delete;
    ScanEngine& operator=(const ScanEngine&) = delete;

    void setScanRange(const ScanRange& range);
    void setScanOptions(const ScanOptions& options);
    void setDeviceStore(std::shared_ptr<DeviceStore> store);

    [[nodiscard]] ScanState state()    const noexcept { return m_state.load(); }
    [[nodiscard]] int       progress() const noexcept { return m_progress.load(); }
    [[nodiscard]] int       totalAddresses()   const;
    [[nodiscard]] int       scannedAddresses() const;
    [[nodiscard]] int       devicesFound()     const;
    [[nodiscard]] qint64    elapsedMs()        const;

public slots:
    void startScan();
    void pauseScan();
    void resumeScan();
    void cancelScan();

signals:
    void stateChanged(ScanState newState);
    void progressChanged(int percent);
    void deviceDiscovered(const Device& device);
    void deviceUpdated(const Device& device);
    void scanStarted();
    void scanFinished(int devicesFound, qint64 elapsedMs);
    void scanError(const QString& message);
    void scanCancelled();

private slots:
    void onWorkerDeviceFound(const Device& device);
    void onWorkerFinished(int workerId);
    void onWorkerError(int workerId, const QString& message);

private:
    void setState(ScanState s);
    void prepareWorkers();
    void distributeAddresses();
    void startWorkerBatch(int batchSize);
    void resolveHostnames();
    void enumerateShares();
    void finalizeScan();
    void mergeDeviceInfo(Device& target, const Device& source);

    [[nodiscard]] QVector<QHostAddress> expandRange(const ScanRange& range) const;
    [[nodiscard]] int optimalThreadCount() const;

    ScanRange                              m_range;
    ScanOptions                            m_options;
    std::shared_ptr<DeviceStore>           m_deviceStore;

    QThreadPool                            m_threadPool;
    QVector<ScanWorker*>                   m_workers;
    QVector<QVector<QHostAddress>>         m_workerAddresses;

    mutable QMutex                         m_mutex;
    std::atomic<ScanState>                 m_state{ScanState::Idle};
    std::atomic<int>                       m_progress{0};
    std::atomic<int>                       m_scannedCount{0};
    std::atomic<int>                       m_foundCount{0};
    int                                    m_totalCount = 0;

    QElapsedTimer                          m_elapsedTimer;

    std::unique_ptr<net::IcmpProbe>        m_icmpProbe;
    std::unique_ptr<net::ArpProbe>         m_arpProbe;
    std::unique_ptr<net::PortScanner>      m_portScanner;
    std::unique_ptr<net::DnsResolver>      m_dnsResolver;
    std::unique_ptr<net::NetbiosQuery>     m_netbiosQuery;
};

} // namespace aips::core
