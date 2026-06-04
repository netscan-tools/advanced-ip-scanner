// Copyright (c) Famatech Corp. All rights reserved.
// Licensed under the proprietary license. See LICENSE for details.

#pragma once

#include <QHostAddress>
#include <QString>
#include <QHash>
#include <QMutex>

#include <optional>

namespace aips::net {

struct DnsRecord {
    QString hostname;
    QString fqdn;
    QVector<QHostAddress> addresses;
    int     ttl = 0;
};

class DnsResolver {
public:
    DnsResolver();
    ~DnsResolver() = default;

    QString resolve(const QHostAddress& address, int timeoutMs = 2000);
    std::optional<DnsRecord> resolveDetailed(const QHostAddress& address, int timeoutMs = 2000);

    QVector<QHostAddress> forwardLookup(const QString& hostname, int timeoutMs = 2000);

    void flushCache();
    void setCacheEnabled(bool enabled) { m_cacheEnabled = enabled; }
    [[nodiscard]] bool isCacheEnabled() const { return m_cacheEnabled; }

    [[nodiscard]] int cacheSize() const;
    [[nodiscard]] uint64_t queriesMade()  const { return m_queriesMade; }
    [[nodiscard]] uint64_t cacheHits()    const { return m_cacheHits; }

private:
    QString performReverseLookup(const QHostAddress& address, int timeoutMs);

    mutable QMutex              m_mutex;
    QHash<quint32, DnsRecord>   m_cache;
    bool                        m_cacheEnabled = true;
    uint64_t                    m_queriesMade  = 0;
    uint64_t                    m_cacheHits    = 0;
};

} // namespace aips::net
