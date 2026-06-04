// Copyright (c) Famatech Corp. All rights reserved.
// Licensed under the proprietary license. See LICENSE for details.

#include "remote/WakeOnLan.h"

#include <QUdpSocket>
#include <QThread>

namespace aips::remote {

WakeOnLan::WakeOnLan(QObject* parent)
    : QObject(parent)
{
}

bool WakeOnLan::wake(const aips::core::MacAddress& mac,
                      const QHostAddress& broadcastAddr,
                      uint16_t port)
{
    QByteArray packet = buildMagicPacket(mac);
    bool ok = sendPacket(packet, broadcastAddr, port);
    emit wakeSent(mac, ok);
    return ok;
}

bool WakeOnLan::wake(const aips::core::Device& device)
{
    return wake(device.macAddress());
}

bool WakeOnLan::wakeWithPassword(const aips::core::MacAddress& mac,
                                   const QByteArray& password,
                                   const QHostAddress& broadcastAddr)
{
    QByteArray packet = buildMagicPacket(mac, password);
    bool ok = sendPacket(packet, broadcastAddr, 9);
    emit wakeSent(mac, ok);
    return ok;
}

int WakeOnLan::wakeMultiple(const QVector<aips::core::Device>& devices, int delayMs)
{
    int sent = 0;
    for (const auto& device : devices) {
        if (wake(device))
            ++sent;

        if (delayMs > 0)
            QThread::msleep(delayMs);
    }
    return sent;
}

QByteArray WakeOnLan::buildMagicPacket(const aips::core::MacAddress& mac)
{
    QByteArray packet;
    packet.reserve(102);

    // 6 bytes of 0xFF
    for (int i = 0; i < 6; ++i)
        packet.append(static_cast<char>(0xFF));

    // MAC address repeated 16 times
    for (int i = 0; i < 16; ++i)
        for (int j = 0; j < 6; ++j)
            packet.append(static_cast<char>(mac[j]));

    return packet;
}

QByteArray WakeOnLan::buildMagicPacket(const aips::core::MacAddress& mac,
                                         const QByteArray& password)
{
    QByteArray packet = buildMagicPacket(mac);

    // Append SecureOn password (4 or 6 bytes)
    if (password.size() == 4 || password.size() == 6)
        packet.append(password);

    return packet;
}

bool WakeOnLan::sendPacket(const QByteArray& packet, const QHostAddress& broadcastAddr,
                            uint16_t port)
{
    QUdpSocket socket;

    socket.bind(QHostAddress::Any, 0);

    qint64 sent = socket.writeDatagram(packet, broadcastAddr, port);

    if (sent != packet.size()) {
        emit wakeError(QString("Failed to send WOL packet: %1").arg(socket.errorString()));
        return false;
    }

    return true;
}

} // namespace aips::remote
