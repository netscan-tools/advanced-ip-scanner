// Copyright (c) Famatech Corp. All rights reserved.
// Licensed under the proprietary license. See LICENSE for details.

#include "remote/RemoteShutdown.h"

#include <QProcess>
#include <QFile>

namespace aips::remote {

RemoteShutdown::RemoteShutdown(QObject* parent)
    : QObject(parent)
{
}

bool RemoteShutdown::execute(const QHostAddress& address, Action action,
                               int delaySec, const QString& message, bool force)
{
    if (!isSupported()) {
        emit actionError(address, "shutdown.exe not available");
        return false;
    }

    QStringList args = buildShutdownArgs(address, action, delaySec, message, force);

    QProcess proc;
    proc.start("shutdown.exe", args);
    proc.waitForFinished(10000);

    bool success = (proc.exitCode() == 0);
    emit actionSent(address, action, success);

    if (!success) {
        QString err = proc.readAllStandardError().trimmed();
        emit actionError(address, err.isEmpty() ? "Unknown shutdown error" : err);
    }

    return success;
}

bool RemoteShutdown::abort(const QHostAddress& address)
{
    QProcess proc;
    QStringList args = {
        "/a",
        QString("/m \\\\%1").arg(address.toString())
    };

    proc.start("shutdown.exe", args);
    proc.waitForFinished(5000);
    return proc.exitCode() == 0;
}

int RemoteShutdown::executeMultiple(const QVector<QHostAddress>& addresses, Action action,
                                      int delaySec, const QString& message, bool force)
{
    int success = 0;
    for (const auto& addr : addresses) {
        if (execute(addr, action, delaySec, message, force))
            ++success;
    }
    return success;
}

bool RemoteShutdown::isSupported()
{
    return QFile::exists("C:\\Windows\\System32\\shutdown.exe");
}

QStringList RemoteShutdown::buildShutdownArgs(const QHostAddress& address, Action action,
                                                int delaySec, const QString& message, bool force)
{
    QStringList args;

    args << QString("/m") << QString("\\\\%1").arg(address.toString());

    switch (action) {
        case Action::Shutdown:
            args << "/s";
            break;
        case Action::Restart:
            args << "/r";
            break;
        case Action::LogOff:
            args << "/l";
            break;
        case Action::Hibernate:
            args << "/h";
            break;
        case Action::Sleep:
            args << "/h" << "/hybrid";
            break;
    }

    if (delaySec > 0)
        args << "/t" << QString::number(delaySec);
    else
        args << "/t" << "0";

    if (!message.isEmpty())
        args << "/c" << message;

    if (force)
        args << "/f";

    return args;
}

} // namespace aips::remote
