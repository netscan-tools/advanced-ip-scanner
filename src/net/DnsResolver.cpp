// Copyright (c) Famatech Corp. All rights reserved.
// Licensed under the proprietary license. See LICENSE for details.

#include "net/DnsResolver.h"

#include <QHostInfo>
#include <QEventLoop>
#include <QTimer>

#ifdef Q_OS_WIN
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#endif

namespace aips::net {

DnsResolver::DnsResolver() = default;

QString DnsResolver::resolve(const QHostAddress& address, int timeoutMs)
{
    auto record = resolveDetailed(address, timeoutMs);
    return record.has_value() ? record->hostname : QString();
}

std::optional<DnsRecord> DnsResolver::resolveDetailed(const QHostAddress& address, int timeoutMs)
{
    ++m_queriesMade;

    if (m_cacheEnabled) {
        QMutexLocker lock(&m_mutex);
        auto it = m_cache.find(address.toIPv4Address());
        if (it != m_cache.end()) {
            ++m_cacheHits;
            return it.value();
        }
    }

    QString hostname = performReverseLookup(address, timeoutMs);
    if (hostname.isEmpty())
        return std::nullopt;

    DnsRecord record;
    record.hostname = hostname;
    record.fqdn     = hostname;
    record.addresses.append(address);

    if (m_cacheEnabled) {
        QMutexLocker lock(&m_mutex);
        m_cache.insert(address.toIPv4Address(), record);
    }

    return record;
}

QVector<QHostAddress> DnsResolver::forwardLookup(const QString& hostname, int timeoutMs)
{
    QVector<QHostAddress> results;

    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);

    QHostInfo::lookupHost(hostname, [&](const QHostInfo& info) {
        if (info.error() == QHostInfo::NoError) {
            for (const auto& addr : info.addresses())
                results.append(addr);
        }
        loop.quit();
    });

    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timer.start(timeoutMs);
    loop.exec();

    return results;
}

void DnsResolver::flushCache()
{
    QMutexLocker lock(&m_mutex);
    m_cache.clear();
}

int DnsResolver::cacheSize() const
{
    QMutexLocker lock(&m_mutex);
    return m_cache.size();
}

QString DnsResolver::performReverseLookup(const QHostAddress& address, int timeoutMs)
{
#ifdef Q_OS_WIN
    struct sockaddr_in sa;
    std::memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = htonl(address.toIPv4Address());

    char hostBuf[NI_MAXHOST] = {};
    int ret = getnameinfo(
        reinterpret_cast<struct sockaddr*>(&sa),
        sizeof(sa),
        hostBuf,
        sizeof(hostBuf),
        nullptr,
        0,
        NI_NAMEREQD
    );

    if (ret == 0)
        return QString::fromLocal8Bit(hostBuf);

    return {};
#else
    Q_UNUSED(timeoutMs);

    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    QString result;

    QHostInfo::lookupHost(address.toString(), [&](const QHostInfo& info) {
        if (info.error() == QHostInfo::NoError)
            result = info.hostName();
        loop.quit();
    });

    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timer.start(timeoutMs);
    loop.exec();

    return result;
#endif
}

} // namespace aips::net
