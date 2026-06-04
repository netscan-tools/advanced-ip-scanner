// Copyright (c) Famatech Corp. All rights reserved.
// Licensed under the proprietary license. See LICENSE for details.

#include "net/PortScanner.h"

#include <QTcpSocket>
#include <QElapsedTimer>
#include <QtConcurrent>
#include <QSemaphore>

namespace aips::net {

const QVector<uint16_t> PortScanner::s_commonPorts = {
    21, 22, 23, 25, 53, 80, 110, 111, 135, 139,
    143, 443, 445, 993, 995, 1723, 3306, 3389,
    5900, 5901, 8080, 8443, 8888, 9090, 27017
};

const QHash<uint16_t, QString> PortScanner::s_serviceMap = {
    {21,    "FTP"},
    {22,    "SSH"},
    {23,    "Telnet"},
    {25,    "SMTP"},
    {53,    "DNS"},
    {80,    "HTTP"},
    {110,   "POP3"},
    {111,   "RPCBind"},
    {135,   "MSRPC"},
    {139,   "NetBIOS-SSN"},
    {143,   "IMAP"},
    {443,   "HTTPS"},
    {445,   "Microsoft-DS"},
    {993,   "IMAPS"},
    {995,   "POP3S"},
    {1723,  "PPTP"},
    {3306,  "MySQL"},
    {3389,  "RDP"},
    {5900,  "VNC"},
    {5901,  "VNC-1"},
    {8080,  "HTTP-Alt"},
    {8443,  "HTTPS-Alt"},
    {8888,  "HTTP-Alt2"},
    {9090,  "WebSM"},
    {27017, "MongoDB"}
};

PortScanner::PortScanner(QObject* parent)
    : QObject(parent)
{
}

QVector<PortResult> PortScanner::scanPorts(const QHostAddress& address,
                                            const QVector<uint16_t>& ports,
                                            int timeoutMs,
                                            int maxConcurrent)
{
    m_cancelled.store(false);

    QVector<PortResult> results;
    results.reserve(ports.size());

    QSemaphore semaphore(maxConcurrent);
    QMutex resultMutex;
    int scanned = 0;

    QVector<QFuture<void>> futures;
    futures.reserve(ports.size());

    for (uint16_t port : ports) {
        if (m_cancelled.load())
            break;

        semaphore.acquire();

        auto future = QtConcurrent::run([&, port]() {
            PortResult result = probePort(address, port, timeoutMs);

            {
                QMutexLocker lock(&resultMutex);
                results.append(result);
                ++scanned;
            }

            emit portScanned(address, result);
            emit scanProgress(scanned, ports.size());

            semaphore.release();
        });

        futures.append(future);
    }

    for (auto& future : futures)
        future.waitForFinished();

    int openCount = 0;
    for (const auto& r : results) {
        if (r.open)
            ++openCount;
    }

    emit scanComplete(address, openCount);
    return results;
}

QVector<PortResult> PortScanner::scanCommonPorts(const QHostAddress& address, int timeoutMs)
{
    return scanPorts(address, s_commonPorts, timeoutMs);
}

QVector<PortResult> PortScanner::scanAllPorts(const QHostAddress& address, int timeoutMs)
{
    QVector<uint16_t> allPorts;
    allPorts.reserve(65535);
    for (int p = 1; p <= 65535; ++p)
        allPorts.append(static_cast<uint16_t>(p));

    return scanPorts(address, allPorts, timeoutMs, 256);
}

PortResult PortScanner::scanSinglePort(const QHostAddress& address, uint16_t port, int timeoutMs)
{
    return probePort(address, port, timeoutMs);
}

void PortScanner::cancelScan()
{
    m_cancelled.store(true);
}

QVector<uint16_t> PortScanner::commonPorts()
{
    return s_commonPorts;
}

QString PortScanner::serviceName(uint16_t port)
{
    auto it = s_serviceMap.find(port);
    return it != s_serviceMap.end() ? it.value() : QString();
}

PortResult PortScanner::probePort(const QHostAddress& address, uint16_t port, int timeoutMs)
{
    PortResult result;
    result.port = port;
    result.serviceName = serviceName(port);

    QTcpSocket socket;
    QElapsedTimer timer;
    timer.start();

    socket.connectToHost(address, port);
    result.open = socket.waitForConnected(timeoutMs);
    result.responseTimeMs = timer.elapsed();

    if (result.open) {
        result.banner = grabBanner(address, port, std::min(timeoutMs, 500));
        socket.disconnectFromHost();
    }

    return result;
}

QString PortScanner::grabBanner(const QHostAddress& address, uint16_t port, int timeoutMs)
{
    QTcpSocket socket;
    socket.connectToHost(address, port);
    if (!socket.waitForConnected(timeoutMs))
        return {};

    if (socket.waitForReadyRead(timeoutMs)) {
        QByteArray data = socket.readAll();
        QString banner = QString::fromUtf8(data).trimmed();
        if (banner.length() > 256)
            banner.truncate(256);
        socket.disconnectFromHost();
        return banner;
    }

    socket.disconnectFromHost();
    return {};
}

} // namespace aips::net
