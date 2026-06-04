// Copyright (c) Famatech Corp. All rights reserved.
// Licensed under the proprietary license. See LICENSE for details.

#pragma once

#include <QObject>
#include <QHostAddress>
#include <QVector>

#include <cstdint>
#include <functional>
#include <atomic>

namespace aips::net {

struct PortResult {
    uint16_t port;
    bool     open;
    QString  serviceName;
    QString  banner;
    double   responseTimeMs;
};

class PortScanner : public QObject {
    Q_OBJECT

public:
    explicit PortScanner(QObject* parent = nullptr);
    ~PortScanner() override = default;

    QVector<PortResult> scanPorts(const QHostAddress& address,
                                  const QVector<uint16_t>& ports,
                                  int timeoutMs = 200,
                                  int maxConcurrent = 64);

    QVector<PortResult> scanCommonPorts(const QHostAddress& address, int timeoutMs = 200);
    QVector<PortResult> scanAllPorts(const QHostAddress& address, int timeoutMs = 100);
    PortResult          scanSinglePort(const QHostAddress& address, uint16_t port, int timeoutMs = 200);

    void cancelScan();

    [[nodiscard]] static QVector<uint16_t> commonPorts();
    [[nodiscard]] static QString serviceName(uint16_t port);

signals:
    void portScanned(const QHostAddress& address, const PortResult& result);
    void scanProgress(int scanned, int total);
    void scanComplete(const QHostAddress& address, int openPorts);

private:
    PortResult probePort(const QHostAddress& address, uint16_t port, int timeoutMs);
    QString    grabBanner(const QHostAddress& address, uint16_t port, int timeoutMs);

    std::atomic<bool> m_cancelled{false};

    static const QVector<uint16_t> s_commonPorts;
    static const QHash<uint16_t, QString> s_serviceMap;
};

} // namespace aips::net
