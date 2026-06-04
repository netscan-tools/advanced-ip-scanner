// Copyright (c) Famatech Corp. All rights reserved.
// Licensed under the proprietary license. See LICENSE for details.

#pragma once

#include "core/Device.h"

#include <QHostAddress>

#include <cstdint>
#include <optional>
#include <unordered_map>
#include <mutex>

namespace aips::net {

struct ArpEntry {
    QHostAddress           ipAddress;
    aips::core::MacAddress macAddress;
    bool                   isStatic;
    QString                interfaceName;
};

class ArpProbe {
public:
    ArpProbe() = default;
    ~ArpProbe() = default;

    std::optional<aips::core::MacAddress> resolve(const QHostAddress& address, int timeoutMs = 300);

    QVector<ArpEntry> getArpTable();
    void              flushCache();

    [[nodiscard]] bool hasCachedEntry(const QHostAddress& address) const;
    [[nodiscard]] std::optional<aips::core::MacAddress> getCachedMac(const QHostAddress& address) const;

private:
    mutable std::mutex m_cacheMutex;
    std::unordered_map<uint32_t, aips::core::MacAddress> m_cache;

    void cacheEntry(const QHostAddress& addr, const aips::core::MacAddress& mac);
};

} // namespace aips::net
