// Copyright (c) Famatech Corp. All rights reserved.
// Licensed under the proprietary license. See LICENSE for details.

#include "core/Device.h"

#include <QJsonArray>

namespace aips::core {

Device::Device()
    : m_macAddress{}
    , m_status(DeviceStatus::Unknown)
    , m_deviceType(DeviceType::Unknown)
{
}

Device::Device(const QHostAddress& address)
    : m_ipAddress(address)
    , m_macAddress{}
    , m_status(DeviceStatus::Unknown)
    , m_deviceType(DeviceType::Unknown)
{
}

Device::Device(const QHostAddress& address, const MacAddress& mac)
    : m_ipAddress(address)
    , m_macAddress(mac)
    , m_status(DeviceStatus::Unknown)
    , m_deviceType(DeviceType::Unknown)
{
}

QString Device::macAddressString() const
{
    return QString("%1:%2:%3:%4:%5:%6")
        .arg(m_macAddress[0], 2, 16, QChar('0'))
        .arg(m_macAddress[1], 2, 16, QChar('0'))
        .arg(m_macAddress[2], 2, 16, QChar('0'))
        .arg(m_macAddress[3], 2, 16, QChar('0'))
        .arg(m_macAddress[4], 2, 16, QChar('0'))
        .arg(m_macAddress[5], 2, 16, QChar('0'))
        .toUpper();
}

QString Device::statusString() const
{
    switch (m_status) {
        case DeviceStatus::Online:      return QStringLiteral("Online");
        case DeviceStatus::Offline:     return QStringLiteral("Offline");
        case DeviceStatus::Filtered:    return QStringLiteral("Filtered");
        case DeviceStatus::Unreachable: return QStringLiteral("Unreachable");
        default:                        return QStringLiteral("Unknown");
    }
}

QString Device::deviceTypeString() const
{
    switch (m_deviceType) {
        case DeviceType::Workstation:  return QStringLiteral("Workstation");
        case DeviceType::Server:       return QStringLiteral("Server");
        case DeviceType::Router:       return QStringLiteral("Router");
        case DeviceType::Switch:       return QStringLiteral("Switch");
        case DeviceType::Printer:      return QStringLiteral("Printer");
        case DeviceType::AccessPoint:  return QStringLiteral("Access Point");
        case DeviceType::NAS:          return QStringLiteral("NAS");
        case DeviceType::IoT:          return QStringLiteral("IoT Device");
        case DeviceType::Mobile:       return QStringLiteral("Mobile");
        case DeviceType::Virtual:      return QStringLiteral("Virtual Machine");
        default:                       return QStringLiteral("Unknown");
    }
}

QJsonObject Device::toJson() const
{
    QJsonObject obj;
    obj["ip"]          = m_ipAddress.toString();
    obj["mac"]         = macAddressString();
    obj["hostname"]    = m_hostname;
    obj["vendor"]      = m_vendor;
    obj["os"]          = m_osFamily;
    obj["netbios"]     = m_netbiosName;
    obj["workgroup"]   = m_workgroup;
    obj["comment"]     = m_comment;
    obj["status"]      = static_cast<int>(m_status);
    obj["type"]        = static_cast<int>(m_deviceType);
    obj["ttl"]         = m_ttl;
    obj["latency"]     = m_latencyMs;
    obj["lastSeen"]    = m_lastSeen.toString(Qt::ISODate);
    obj["favorite"]    = m_favorite;

    QJsonArray ports;
    for (uint16_t port : m_openPorts)
        ports.append(static_cast<int>(port));
    obj["openPorts"] = ports;

    QJsonArray shares;
    for (const auto& res : m_sharedResources) {
        QJsonObject shareObj;
        shareObj["protocol"]     = static_cast<int>(res.protocol);
        shareObj["path"]         = res.path;
        shareObj["displayName"]  = res.displayName;
        shareObj["port"]         = static_cast<int>(res.port);
        shareObj["requiresAuth"] = res.requiresAuth;
        shares.append(shareObj);
    }
    obj["shares"] = shares;

    return obj;
}

Device Device::fromJson(const QJsonObject& obj)
{
    Device dev;
    dev.setIpAddress(QHostAddress(obj["ip"].toString()));
    dev.setHostname(obj["hostname"].toString());
    dev.setVendor(obj["vendor"].toString());
    dev.setOsFamily(obj["os"].toString());
    dev.setNetbiosName(obj["netbios"].toString());
    dev.setWorkgroup(obj["workgroup"].toString());
    dev.setComment(obj["comment"].toString());
    dev.setStatus(static_cast<DeviceStatus>(obj["status"].toInt()));
    dev.setDeviceType(static_cast<DeviceType>(obj["type"].toInt()));
    dev.setTtl(obj["ttl"].toInt());
    dev.setLatencyMs(obj["latency"].toDouble());
    dev.setLastSeen(QDateTime::fromString(obj["lastSeen"].toString(), Qt::ISODate));
    dev.setFavorite(obj["favorite"].toBool());

    const QString macStr = obj["mac"].toString();
    if (!macStr.isEmpty()) {
        MacAddress mac{};
        const QStringList parts = macStr.split(':');
        if (parts.size() == 6) {
            for (int i = 0; i < 6; ++i)
                mac[i] = static_cast<uint8_t>(parts[i].toUInt(nullptr, 16));
            dev.setMacAddress(mac);
        }
    }

    const QJsonArray ports = obj["openPorts"].toArray();
    for (const auto& p : ports)
        dev.addOpenPort(static_cast<uint16_t>(p.toInt()));

    const QJsonArray shares = obj["shares"].toArray();
    for (const auto& s : shares) {
        QJsonObject so = s.toObject();
        SharedResource res;
        res.protocol     = static_cast<SharedResource::Protocol>(so["protocol"].toInt());
        res.path         = so["path"].toString();
        res.displayName  = so["displayName"].toString();
        res.port         = static_cast<uint16_t>(so["port"].toInt());
        res.requiresAuth = so["requiresAuth"].toBool();
        dev.addSharedResource(res);
    }

    return dev;
}

bool Device::operator==(const Device& other) const
{
    return m_ipAddress == other.m_ipAddress;
}

bool Device::operator<(const Device& other) const
{
    return m_ipAddress.toIPv4Address() < other.m_ipAddress.toIPv4Address();
}

} // namespace aips::core
