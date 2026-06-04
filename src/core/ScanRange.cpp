// Copyright (c) Famatech Corp. All rights reserved.
// Licensed under the proprietary license. See LICENSE for details.

#include "core/ScanRange.h"

#include <QFile>
#include <QTextStream>
#include <QRegularExpression>

#include <algorithm>

namespace aips::core {

ScanRange ScanRange::fromSingleAddress(const QHostAddress& addr)
{
    ScanRange r;
    r.m_type         = Type::SingleAddress;
    r.m_startAddress = addr;
    r.m_endAddress   = addr;
    return r;
}

ScanRange ScanRange::fromRange(const QHostAddress& start, const QHostAddress& end)
{
    ScanRange r;
    r.m_type         = Type::AddressRange;
    r.m_startAddress = start;
    r.m_endAddress   = end;
    return r;
}

ScanRange ScanRange::fromSubnet(const QHostAddress& network, int prefixLength)
{
    ScanRange r;
    r.m_type         = Type::Subnet;
    r.m_startAddress = network;
    r.m_prefixLength = prefixLength;

    uint32_t netAddr  = toUint32(network);
    uint32_t mask     = prefixLength == 0 ? 0 : (~uint32_t(0)) << (32 - prefixLength);
    uint32_t hostMask = ~mask;

    r.m_startAddress = fromUint32((netAddr & mask) + 1);
    r.m_endAddress   = fromUint32((netAddr | hostMask) - 1);

    return r;
}

ScanRange ScanRange::fromCIDR(const QString& cidr)
{
    static QRegularExpression re(R"(^(\d+\.\d+\.\d+\.\d+)/(\d+)$)");
    auto match = re.match(cidr.trimmed());
    if (!match.hasMatch())
        return {};

    QHostAddress addr(match.captured(1));
    int prefix = match.captured(2).toInt();

    if (addr.isNull() || prefix < 0 || prefix > 32)
        return {};

    return fromSubnet(addr, prefix);
}

ScanRange ScanRange::fromAddressList(const QVector<QHostAddress>& addresses)
{
    ScanRange r;
    r.m_type       = Type::CustomList;
    r.m_customList = addresses;
    if (!addresses.isEmpty()) {
        r.m_startAddress = addresses.first();
        r.m_endAddress   = addresses.last();
    }
    return r;
}

ScanRange ScanRange::fromFile(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return {};

    QVector<QHostAddress> addresses;
    QTextStream in(&file);

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty() || line.startsWith('#'))
            continue;

        if (line.contains('/')) {
            ScanRange subRange = fromCIDR(line);
            if (subRange.isValid())
                addresses.append(subRange.expandAll());
            continue;
        }

        if (line.contains('-')) {
            QStringList parts = line.split('-');
            if (parts.size() == 2) {
                QHostAddress start(parts[0].trimmed());
                QHostAddress end(parts[1].trimmed());
                if (!start.isNull() && !end.isNull()) {
                    ScanRange subRange = fromRange(start, end);
                    addresses.append(subRange.expandAll());
                }
            }
            continue;
        }

        QHostAddress addr(line);
        if (!addr.isNull())
            addresses.append(addr);
    }

    return fromAddressList(addresses);
}

QVector<QHostAddress> ScanRange::expandAll() const
{
    QVector<QHostAddress> result;

    if (m_type == Type::CustomList)
        return m_customList;

    uint32_t start = toUint32(m_startAddress);
    uint32_t end   = toUint32(m_endAddress);

    if (start > end)
        std::swap(start, end);

    result.reserve(static_cast<int>(end - start + 1));
    for (uint32_t ip = start; ip <= end; ++ip)
        result.append(fromUint32(ip));

    return result;
}

uint32_t ScanRange::totalAddresses() const
{
    if (m_type == Type::CustomList)
        return static_cast<uint32_t>(m_customList.size());

    uint32_t start = toUint32(m_startAddress);
    uint32_t end   = toUint32(m_endAddress);

    return (start <= end) ? (end - start + 1) : (start - end + 1);
}

bool ScanRange::contains(const QHostAddress& addr) const
{
    if (m_type == Type::CustomList)
        return m_customList.contains(addr);

    uint32_t ip    = toUint32(addr);
    uint32_t start = toUint32(m_startAddress);
    uint32_t end   = toUint32(m_endAddress);

    return ip >= start && ip <= end;
}

bool ScanRange::isValid() const
{
    if (m_type == Type::CustomList)
        return !m_customList.isEmpty();

    return !m_startAddress.isNull() && !m_endAddress.isNull();
}

QString ScanRange::toString() const
{
    switch (m_type) {
        case Type::SingleAddress:
            return m_startAddress.toString();
        case Type::AddressRange:
            return QString("%1-%2").arg(m_startAddress.toString(), m_endAddress.toString());
        case Type::Subnet:
            return QString("%1/%2").arg(m_startAddress.toString()).arg(m_prefixLength);
        case Type::ClassC:
            return QString("%1/24").arg(m_startAddress.toString());
        case Type::CustomList:
            return QString("[%1 addresses]").arg(m_customList.size());
    }
    return {};
}

std::optional<ScanRange> ScanRange::parse(const QString& text)
{
    QString trimmed = text.trimmed();
    if (trimmed.isEmpty())
        return std::nullopt;

    if (trimmed.contains('/'))
        return fromCIDR(trimmed);

    if (trimmed.contains('-')) {
        QStringList parts = trimmed.split('-');
        if (parts.size() == 2) {
            QHostAddress start(parts[0].trimmed());
            QHostAddress end(parts[1].trimmed());
            if (!start.isNull() && !end.isNull())
                return fromRange(start, end);
        }
        return std::nullopt;
    }

    QHostAddress addr(trimmed);
    if (!addr.isNull())
        return fromSingleAddress(addr);

    return std::nullopt;
}

bool ScanRange::operator==(const ScanRange& other) const
{
    return m_type == other.m_type
        && m_startAddress == other.m_startAddress
        && m_endAddress == other.m_endAddress
        && m_prefixLength == other.m_prefixLength;
}

uint32_t ScanRange::toUint32(const QHostAddress& addr)
{
    return addr.toIPv4Address();
}

QHostAddress ScanRange::fromUint32(uint32_t val)
{
    return QHostAddress(val);
}

} // namespace aips::core
