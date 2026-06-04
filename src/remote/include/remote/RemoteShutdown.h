// Copyright (c) Famatech Corp. All rights reserved.
// Licensed under the proprietary license. See LICENSE for details.

#pragma once

#include <QObject>
#include <QHostAddress>

namespace aips::remote {

class RemoteShutdown : public QObject {
    Q_OBJECT

public:
    enum class Action : uint8_t {
        Shutdown,
        Restart,
        LogOff,
        Hibernate,
        Sleep
    };
    Q_ENUM(Action)

    explicit RemoteShutdown(QObject* parent = nullptr);
    ~RemoteShutdown() override = default;

    bool execute(const QHostAddress& address, Action action,
                  int delaySec = 0, const QString& message = {},
                  bool force = false);

    bool abort(const QHostAddress& address);

    int executeMultiple(const QVector<QHostAddress>& addresses, Action action,
                         int delaySec = 0, const QString& message = {},
                         bool force = false);

    [[nodiscard]] static bool isSupported();

signals:
    void actionSent(const QHostAddress& address, Action action, bool success);
    void actionError(const QHostAddress& address, const QString& error);

private:
    QStringList buildShutdownArgs(const QHostAddress& address, Action action,
                                   int delaySec, const QString& message, bool force);
};

} // namespace aips::remote
