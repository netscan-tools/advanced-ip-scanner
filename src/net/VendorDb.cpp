// Copyright (c) Famatech Corp. All rights reserved.
// Licensed under the proprietary license. See LICENSE for details.

#include "net/VendorDb.h"

#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QDebug>

namespace aips::net {

VendorDb& VendorDb::instance()
{
    static VendorDb db;
    return db;
}

bool VendorDb::loadFromFile(const QString& ouiFilePath)
{
    QMutexLocker lock(&m_mutex);

    QFile file(ouiFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    m_entries.clear();

    QTextStream in(&file);
    while (!in.atEnd()) {
        OuiEntry entry;
        if (parseLine(in.readLine(), entry)) {
            uint32_t key = prefixKey(entry.prefix);
            m_entries.insert(key, entry);
        }
    }

    m_loaded = !m_entries.isEmpty();
    qDebug() << "VendorDb: loaded" << m_entries.size() << "OUI entries";
    return m_loaded;
}

bool VendorDb::loadFromResource()
{
    return loadFromFile(QStringLiteral(":/data/oui.txt"));
}

QString VendorDb::lookupVendor(const aips::core::MacAddress& mac) const
{
    QMutexLocker lock(&m_mutex);
    uint32_t key = prefixKey(mac);
    auto it = m_entries.find(key);
    return it != m_entries.end() ? it->vendorName : QString();
}

QString VendorDb::lookupVendor(const QString& macPrefix) const
{
    QMutexLocker lock(&m_mutex);

    static QRegularExpression re(R"([^0-9A-Fa-f])");
    QString clean = macPrefix.left(8).remove(re);

    if (clean.length() < 6)
        return {};

    std::array<uint8_t, 3> prefix;
    prefix[0] = static_cast<uint8_t>(clean.mid(0, 2).toUInt(nullptr, 16));
    prefix[1] = static_cast<uint8_t>(clean.mid(2, 2).toUInt(nullptr, 16));
    prefix[2] = static_cast<uint8_t>(clean.mid(4, 2).toUInt(nullptr, 16));

    uint32_t key = prefixKey(prefix);
    auto it = m_entries.find(key);
    return it != m_entries.end() ? it->vendorName : QString();
}

int VendorDb::entryCount() const
{
    QMutexLocker lock(&m_mutex);
    return m_entries.size();
}

uint32_t VendorDb::prefixKey(const std::array<uint8_t, 3>& prefix)
{
    return (static_cast<uint32_t>(prefix[0]) << 16) |
           (static_cast<uint32_t>(prefix[1]) << 8)  |
            static_cast<uint32_t>(prefix[2]);
}

uint32_t VendorDb::prefixKey(const aips::core::MacAddress& mac)
{
    return (static_cast<uint32_t>(mac[0]) << 16) |
           (static_cast<uint32_t>(mac[1]) << 8)  |
            static_cast<uint32_t>(mac[2]);
}

bool VendorDb::parseLine(const QString& line, OuiEntry& entry)
{
    static QRegularExpression re(
        R"(^([0-9A-Fa-f]{2})-([0-9A-Fa-f]{2})-([0-9A-Fa-f]{2})\s+\(hex\)\s+(.+)$)"
    );

    auto match = re.match(line.trimmed());
    if (!match.hasMatch())
        return false;

    entry.prefix[0] = static_cast<uint8_t>(match.captured(1).toUInt(nullptr, 16));
    entry.prefix[1] = static_cast<uint8_t>(match.captured(2).toUInt(nullptr, 16));
    entry.prefix[2] = static_cast<uint8_t>(match.captured(3).toUInt(nullptr, 16));
    entry.vendorName = match.captured(4).trimmed();
    entry.shortName  = entry.vendorName.section(' ', 0, 0);

    return true;
}

} // namespace aips::net
