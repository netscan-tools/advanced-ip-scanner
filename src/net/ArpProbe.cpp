// Copyright (c) Famatech Corp. All rights reserved.
// Licensed under the proprietary license. See LICENSE for details.

#include "net/ArpProbe.h"

#include <algorithm>

#ifdef Q_OS_WIN
#include <winsock2.h>
#include <iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")
#endif

namespace aips::net {

std::optional<aips::core::MacAddress> ArpProbe::resolve(const QHostAddress& address, int timeoutMs)
{
    Q_UNUSED(timeoutMs);

    {
        std::lock_guard lock(m_cacheMutex);
        auto it = m_cache.find(address.toIPv4Address());
        if (it != m_cache.end())
            return it->second;
    }

#ifdef Q_OS_WIN
    ULONG macBuf[2] = {};
    ULONG macLen     = 6;

    DWORD ret = SendARP(
        htonl(address.toIPv4Address()),
        0,
        macBuf,
        &macLen
    );

    if (ret == NO_ERROR && macLen == 6) {
        aips::core::MacAddress mac{};
        auto* bytes = reinterpret_cast<uint8_t*>(macBuf);
        std::copy(bytes, bytes + 6, mac.begin());
        cacheEntry(address, mac);
        return mac;
    }
#endif

    return std::nullopt;
}

QVector<ArpEntry> ArpProbe::getArpTable()
{
    QVector<ArpEntry> entries;

#ifdef Q_OS_WIN
    ULONG tableSize = 0;
    GetIpNetTable(nullptr, &tableSize, FALSE);

    auto buffer = std::make_unique<BYTE[]>(tableSize);
    auto* table = reinterpret_cast<PMIB_IPNETTABLE>(buffer.get());

    if (GetIpNetTable(table, &tableSize, TRUE) == NO_ERROR) {
        for (DWORD i = 0; i < table->dwNumEntries; ++i) {
            const MIB_IPNETROW& row = table->table[i];

            if (row.dwType == MIB_IPNET_TYPE_INVALID)
                continue;

            ArpEntry entry;
            entry.ipAddress = QHostAddress(ntohl(row.dwAddr));
            entry.isStatic  = (row.dwType == MIB_IPNET_TYPE_STATIC);

            if (row.dwPhysAddrLen == 6) {
                for (int j = 0; j < 6; ++j)
                    entry.macAddress[j] = row.bPhysAddr[j];
            }

            entries.append(entry);
        }
    }
#endif

    return entries;
}

void ArpProbe::flushCache()
{
    std::lock_guard lock(m_cacheMutex);
    m_cache.clear();
}

bool ArpProbe::hasCachedEntry(const QHostAddress& address) const
{
    std::lock_guard lock(m_cacheMutex);
    return m_cache.count(address.toIPv4Address()) > 0;
}

std::optional<aips::core::MacAddress> ArpProbe::getCachedMac(const QHostAddress& address) const
{
    std::lock_guard lock(m_cacheMutex);
    auto it = m_cache.find(address.toIPv4Address());
    return it != m_cache.end() ? std::optional(it->second) : std::nullopt;
}

void ArpProbe::cacheEntry(const QHostAddress& addr, const aips::core::MacAddress& mac)
{
    std::lock_guard lock(m_cacheMutex);
    m_cache[addr.toIPv4Address()] = mac;
}

} // namespace aips::net
