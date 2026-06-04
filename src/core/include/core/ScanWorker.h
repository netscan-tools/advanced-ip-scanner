// Copyright (c) Famatech Corp. All rights reserved.
// Licensed under the proprietary license. See LICENSE for details.

#pragma once

#include "core/Device.h"
#include "core/ScanOptions.h"

#include <QRunnable>
#include <QObject>
#include <QHostAddress>
#include <QVector>

#include <atomic>

namespace aips::net {
    class IcmpProbe;
    class ArpProbe;
}

namespace aips::core {

class ScanWorker : public QObject, public QRunnable {
    Q_OBJECT

public:
    ScanWorker(int workerId,
               const QVector<QHostAddress>& addresses,
               const ScanOptions& options,
               QObject* parent = nullptr);
    ~ScanWorker() override;

    void run() override;
    void requestCancel();

    [[nodiscard]] int  workerId()  const noexcept { return m_workerId; }
    [[nodiscard]] int  scanned()   const noexcept { return m_scanned.load(); }
    [[nodiscard]] int  found()     const noexcept { return m_found.load(); }
    [[nodiscard]] bool isCancelled() const noexcept { return m_cancelled.load(); }

signals:
    void deviceFound(const aips::core::Device& device);
    void progressUpdate(int workerId, int scanned, int total);
    void finished(int workerId);
    void error(int workerId, const QString& message);

private:
    Device probeAddress(const QHostAddress& addr);
    bool   pingHost(const QHostAddress& addr, int timeoutMs, int retries);
    bool   arpResolve(const QHostAddress& addr, MacAddress& outMac);
    void   throttle() const;

    int                     m_workerId;
    QVector<QHostAddress>   m_addresses;
    ScanOptions             m_options;
    std::atomic<int>        m_scanned{0};
    std::atomic<int>        m_found{0};
    std::atomic<bool>       m_cancelled{false};
};

} // namespace aips::core
