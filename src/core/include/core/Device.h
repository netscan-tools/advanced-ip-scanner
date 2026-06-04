// Copyright (c) Famatech Corp. All rights reserved.
// Licensed under the proprietary license. See LICENSE for details.

#pragma once

#include <QString>
#include <QHostAddress>
#include <QDateTime>
#include <QJsonObject>
#include <QMetaType>

#include <cstdint>
#include <array>
#include <optional>

namespace aips::core {

using MacAddress = std::array<uint8_t, 6>;

enum class DeviceStatus : uint8_t {
    Unknown    = 0,
    Online     = 1,
    Offline    = 2,
    Filtered   = 3,
    Unreachable = 4
};

enum class DeviceType : uint8_t {
    Unknown    = 0,
    Workstation,
    Server,
    Router,
    Switch,
    Printer,
    AccessPoint,
    NAS,
    IoT,
    Mobile,
    Virtual
};

struct SharedResource {
    enum class Protocol : uint8_t { SMB, FTP, HTTP, HTTPS };

    Protocol    protocol;
    QString     path;
    QString     displayName;
    uint16_t    port;
    bool        requiresAuth;
};

class Device {
public:
    Device();
    explicit Device(const QHostAddress& address);
    Device(const QHostAddress& address, const MacAddress& mac);
    ~Device() = default;

    Device(const Device&) = default;
    Device& operator=(const Device&) = default;
    Device(Device&&) noexcept = default;
    Device& operator=(Device&&) noexcept = default;

    [[nodiscard]] const QHostAddress&  ipAddress()    const noexcept { return m_ipAddress; }
    [[nodiscard]] const MacAddress&    macAddress()   const noexcept { return m_macAddress; }
    [[nodiscard]] const QString&       hostname()     const noexcept { return m_hostname; }
    [[nodiscard]] const QString&       vendor()       const noexcept { return m_vendor; }
    [[nodiscard]] const QString&       osFamily()     const noexcept { return m_osFamily; }
    [[nodiscard]] const QString&       netbiosName()  const noexcept { return m_netbiosName; }
    [[nodiscard]] const QString&       workgroup()    const noexcept { return m_workgroup; }
    [[nodiscard]] const QString&       comment()      const noexcept { return m_comment; }
    [[nodiscard]] DeviceStatus         status()       const noexcept { return m_status; }
    [[nodiscard]] DeviceType           deviceType()   const noexcept { return m_deviceType; }
    [[nodiscard]] int                  ttl()          const noexcept { return m_ttl; }
    [[nodiscard]] double               latencyMs()    const noexcept { return m_latencyMs; }
    [[nodiscard]] const QDateTime&     lastSeen()     const noexcept { return m_lastSeen; }
    [[nodiscard]] bool                 isFavorite()   const noexcept { return m_favorite; }

    [[nodiscard]] const QVector<SharedResource>& sharedResources() const noexcept { return m_sharedResources; }
    [[nodiscard]] const QVector<uint16_t>&       openPorts()       const noexcept { return m_openPorts; }

    void setIpAddress(const QHostAddress& addr)         { m_ipAddress = addr; }
    void setMacAddress(const MacAddress& mac)           { m_macAddress = mac; }
    void setHostname(const QString& name)               { m_hostname = name; }
    void setVendor(const QString& vendor)               { m_vendor = vendor; }
    void setOsFamily(const QString& os)                 { m_osFamily = os; }
    void setNetbiosName(const QString& name)            { m_netbiosName = name; }
    void setWorkgroup(const QString& wg)                { m_workgroup = wg; }
    void setComment(const QString& comment)             { m_comment = comment; }
    void setStatus(DeviceStatus s)                      { m_status = s; }
    void setDeviceType(DeviceType t)                    { m_deviceType = t; }
    void setTtl(int ttl)                                { m_ttl = ttl; }
    void setLatencyMs(double ms)                        { m_latencyMs = ms; }
    void setLastSeen(const QDateTime& dt)               { m_lastSeen = dt; }
    void setFavorite(bool fav)                           { m_favorite = fav; }

    void addSharedResource(const SharedResource& res)   { m_sharedResources.append(res); }
    void addOpenPort(uint16_t port)                     { m_openPorts.append(port); }
    void clearSharedResources()                         { m_sharedResources.clear(); }
    void clearOpenPorts()                               { m_openPorts.clear(); }

    [[nodiscard]] QString macAddressString() const;
    [[nodiscard]] QString statusString() const;
    [[nodiscard]] QString deviceTypeString() const;

    [[nodiscard]] QJsonObject toJson() const;
    [[nodiscard]] static Device fromJson(const QJsonObject& obj);

    bool operator==(const Device& other) const;
    bool operator<(const Device& other) const;

private:
    QHostAddress            m_ipAddress;
    MacAddress              m_macAddress{};
    QString                 m_hostname;
    QString                 m_vendor;
    QString                 m_osFamily;
    QString                 m_netbiosName;
    QString                 m_workgroup;
    QString                 m_comment;
    DeviceStatus            m_status     = DeviceStatus::Unknown;
    DeviceType              m_deviceType = DeviceType::Unknown;
    int                     m_ttl        = 0;
    double                  m_latencyMs  = 0.0;
    QDateTime               m_lastSeen;
    bool                    m_favorite   = false;
    QVector<SharedResource> m_sharedResources;
    QVector<uint16_t>       m_openPorts;
};

} // namespace aips::core

Q_DECLARE_METATYPE(aips::core::Device)
Q_DECLARE_METATYPE(aips::core::DeviceStatus)
