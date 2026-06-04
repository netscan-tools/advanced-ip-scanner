// Copyright (c) Famatech Corp. All rights reserved.
// Licensed under the proprietary license. See LICENSE for details.

#pragma once

#include <QHostAddress>
#include <QString>
#include <QVector>
#include <QPair>

#include <cstdint>
#include <optional>

namespace aips::core {

class ScanRange {
public:
    enum class Type : uint8_t {
        SingleAddress,
        AddressRange,
        Subnet,
        ClassC,
        CustomList
    };

    ScanRange() = default;

    static ScanRange fromSingleAddress(const QHostAddress& addr);
    static ScanRange fromRange(const QHostAddress& start, const QHostAddress& end);
    static ScanRange fromSubnet(const QHostAddress& network, int prefixLength);
    static ScanRange fromCIDR(const QString& cidr);
    static ScanRange fromAddressList(const QVector<QHostAddress>& addresses);
    static ScanRange fromFile(const QString& filePath);

    [[nodiscard]] Type type() const noexcept { return m_type; }

    [[nodiscard]] QHostAddress startAddress() const { return m_startAddress; }
    [[nodiscard]] QHostAddress endAddress()   const { return m_endAddress; }
    [[nodiscard]] int          prefixLength() const { return m_prefixLength; }

    [[nodiscard]] QVector<QHostAddress> expandAll() const;
    [[nodiscard]] uint32_t totalAddresses() const;

    [[nodiscard]] bool contains(const QHostAddress& addr) const;
    [[nodiscard]] bool isValid() const;

    [[nodiscard]] QString toString() const;
    [[nodiscard]] static std::optional<ScanRange> parse(const QString& text);

    bool operator==(const ScanRange& other) const;

private:
    Type                    m_type = Type::SingleAddress;
    QHostAddress            m_startAddress;
    QHostAddress            m_endAddress;
    int                     m_prefixLength = 24;
    QVector<QHostAddress>   m_customList;

    static uint32_t toUint32(const QHostAddress& addr);
    static QHostAddress fromUint32(uint32_t val);
};

} // namespace aips::core
