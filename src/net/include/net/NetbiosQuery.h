// Copyright (c) Famatech Corp. All rights reserved.
// Licensed under the proprietary license. See LICENSE for details.

#pragma once

#include <QHostAddress>
#include <QString>
#include <QVector>

#include <optional>

namespace aips::net {

struct NetbiosInfo {
    QString name;
    QString workgroup;
    QString domain;
    bool    isFileServer   = false;
    bool    isPrintServer  = false;
    bool    isMasterBrowser = false;
};

struct SmbShareInfo {
    QString  name;
    QString  path;
    QString  comment;
    uint32_t shareType;
    bool     requiresAuth;
};

class NetbiosQuery {
public:
    NetbiosQuery() = default;
    ~NetbiosQuery() = default;

    std::optional<NetbiosInfo> query(const QHostAddress& address, int timeoutMs = 1500);
    QVector<SmbShareInfo>      enumerateShares(const QHostAddress& address);

    QVector<NetbiosInfo> browserQuery(const QHostAddress& subnetBroadcast);

private:
    bool sendNetbiosNameQuery(const QHostAddress& address, int timeoutMs,
                              QByteArray& response);
    NetbiosInfo parseNameQueryResponse(const QByteArray& response);

    static constexpr uint16_t NETBIOS_NAME_PORT = 137;
    static constexpr uint16_t NETBIOS_SESSION_PORT = 139;
    static constexpr uint16_t SMB_PORT = 445;
};

} // namespace aips::net
