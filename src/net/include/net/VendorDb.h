// Copyright (c) Famatech Corp. All rights reserved.
// Licensed under the proprietary license. See LICENSE for details.

#pragma once

#include "core/Device.h"

#include <QString>
#include <QHash>
#include <QMutex>

#include <array>

namespace aips::net {

class VendorDb {
public:
    static VendorDb& instance();

    bool loadFromFile(const QString& ouiFilePath);
    bool loadFromResource();

    [[nodiscard]] QString lookupVendor(const aips::core::MacAddress& mac) const;
    [[nodiscard]] QString lookupVendor(const QString& macPrefix) const;

    [[nodiscard]] int    entryCount() const;
    [[nodiscard]] bool   isLoaded() const { return m_loaded; }

private:
    VendorDb() = default;
    ~VendorDb() = default;
    VendorDb(const VendorDb&) = delete;
    VendorDb& operator=(const VendorDb&) = delete;

    struct OuiEntry {
        std::array<uint8_t, 3> prefix;
        QString                vendorName;
        QString                shortName;
    };

    mutable QMutex         m_mutex;
    QHash<uint32_t, OuiEntry> m_entries;
    bool                   m_loaded = false;

    static uint32_t prefixKey(const std::array<uint8_t, 3>& prefix);
    static uint32_t prefixKey(const aips::core::MacAddress& mac);
    bool parseLine(const QString& line, OuiEntry& entry);
};

} // namespace aips::net
