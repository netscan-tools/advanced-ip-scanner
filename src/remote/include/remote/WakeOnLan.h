// Copyright (c) Famatech Corp. All rights reserved.
// Licensed under the proprietary license. See LICENSE for details.

#pragma once

#include "core/Device.h"

#include <QObject>
#include <QHostAddress>

namespace aips::remote {

class WakeOnLan : public QObject {
    Q_OBJECT

public:
    explicit WakeOnLan(QObject* parent = nullptr);
    ~WakeOnLan() override = default;

    bool wake(const aips::core::MacAddress& mac,
              const QHostAddress& broadcastAddr = QHostAddress::Broadcast,
              uint16_t port = 9);

    bool wake(const aips::core::Device& device);

    bool wakeWithPassword(const aips::core::MacAddress& mac,
                           const QByteArray& password,
                           const QHostAddress& broadcastAddr = QHostAddress::Broadcast);

    int wakeMultiple(const QVector<aips::core::Device>& devices, int delayMs = 100);

    [[nodiscard]] static QByteArray buildMagicPacket(const aips::core::MacAddress& mac);
    [[nodiscard]] static QByteArray buildMagicPacket(const aips::core::MacAddress& mac,
                                                      const QByteArray& password);

signals:
    void wakeSent(const aips::core::MacAddress& mac, bool success);
    void wakeError(const QString& message);

private:
    bool sendPacket(const QByteArray& packet, const QHostAddress& broadcastAddr, uint16_t port);
};

} // namespace aips::remote
