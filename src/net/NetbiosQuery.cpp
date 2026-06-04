// Copyright (c) Famatech Corp. All rights reserved.
// Licensed under the proprietary license. See LICENSE for details.

#include "net/NetbiosQuery.h"

#include <QUdpSocket>
#include <QElapsedTimer>
#include <QDataStream>

#include <cstring>

#ifdef Q_OS_WIN
#include <winsock2.h>
#include <lm.h>
#pragma comment(lib, "netapi32.lib")
#endif

namespace aips::net {

std::optional<NetbiosInfo> NetbiosQuery::query(const QHostAddress& address, int timeoutMs)
{
    QByteArray response;
    if (!sendNetbiosNameQuery(address, timeoutMs, response))
        return std::nullopt;

    return parseNameQueryResponse(response);
}

QVector<SmbShareInfo> NetbiosQuery::enumerateShares(const QHostAddress& address)
{
    QVector<SmbShareInfo> shares;

#ifdef Q_OS_WIN
    PSHARE_INFO_1 shareInfo = nullptr;
    DWORD entriesRead  = 0;
    DWORD totalEntries = 0;

    QString serverPath = QString("\\\\%1").arg(address.toString());
    std::wstring wPath = serverPath.toStdWString();

    NET_API_STATUS status = NetShareEnum(
        const_cast<LPWSTR>(wPath.c_str()),
        1,
        reinterpret_cast<LPBYTE*>(&shareInfo),
        MAX_PREFERRED_LENGTH,
        &entriesRead,
        &totalEntries,
        nullptr
    );

    if (status == NERR_Success && shareInfo != nullptr) {
        for (DWORD i = 0; i < entriesRead; ++i) {
            if (shareInfo[i].shi1_type == STYPE_IPC)
                continue;

            SmbShareInfo info;
            info.name         = QString::fromWCharArray(shareInfo[i].shi1_netname);
            info.path         = QString("\\\\%1\\%2").arg(address.toString(), info.name);
            info.comment      = QString::fromWCharArray(shareInfo[i].shi1_remark);
            info.shareType    = shareInfo[i].shi1_type;
            info.requiresAuth = true;
            shares.append(info);
        }
        NetApiBufferFree(shareInfo);
    }
#else
    Q_UNUSED(address);
#endif

    return shares;
}

QVector<NetbiosInfo> NetbiosQuery::browserQuery(const QHostAddress& subnetBroadcast)
{
    QVector<NetbiosInfo> results;

    QByteArray response;
    if (sendNetbiosNameQuery(subnetBroadcast, 3000, response)) {
        auto info = parseNameQueryResponse(response);
        if (info.has_value())
            results.append(info.value());
    }

    return results;
}

bool NetbiosQuery::sendNetbiosNameQuery(const QHostAddress& address, int timeoutMs,
                                         QByteArray& response)
{
    QUdpSocket socket;
    socket.bind(QHostAddress::Any, 0);

    // NBSTAT query packet: wildcard name query
    QByteArray packet;
    packet.resize(50);
    std::memset(packet.data(), 0, 50);

    // Transaction ID
    packet[0] = 0x80;
    packet[1] = 0x01;

    // Flags: standard query
    packet[2] = 0x00;
    packet[3] = 0x00;

    // Questions count = 1
    packet[4] = 0x00;
    packet[5] = 0x01;

    // Encoded wildcard name "*"
    packet[12] = 0x20;
    for (int i = 0; i < 32; ++i)
        packet[13 + i] = (i == 0) ? 'C' : 'K';
    // NBSTAT query type
    packet[45] = 0x00;
    packet[46] = 0x21;
    packet[47] = 0x00;
    packet[48] = 0x01;

    socket.writeDatagram(packet, address, NETBIOS_NAME_PORT);

    QElapsedTimer timer;
    timer.start();

    while (timer.elapsed() < timeoutMs) {
        if (socket.waitForReadyRead(100)) {
            response = socket.receiveDatagram().data();
            if (response.size() > 56)
                return true;
        }
    }

    return false;
}

NetbiosInfo NetbiosQuery::parseNameQueryResponse(const QByteArray& response)
{
    NetbiosInfo info;

    if (response.size() < 57)
        return info;

    int nameCount = static_cast<uint8_t>(response[56]);
    int offset = 57;

    for (int i = 0; i < nameCount && offset + 18 <= response.size(); ++i) {
        QByteArray nameBytes = response.mid(offset, 15);
        uint8_t    nameType  = static_cast<uint8_t>(response[offset + 15]);
        uint16_t   flags     = static_cast<uint16_t>(
            (static_cast<uint8_t>(response[offset + 16]) << 8) |
             static_cast<uint8_t>(response[offset + 17])
        );

        QString name = QString::fromLatin1(nameBytes).trimmed();
        bool isGroup = (flags & 0x8000) != 0;

        switch (nameType) {
            case 0x00:
                if (!isGroup && info.name.isEmpty())
                    info.name = name;
                if (isGroup)
                    info.workgroup = name;
                break;
            case 0x03:
                break;
            case 0x20:
                info.isFileServer = true;
                break;
            case 0x1D:
                info.isMasterBrowser = true;
                break;
            default:
                break;
        }

        offset += 18;
    }

    return info;
}

} // namespace aips::net
