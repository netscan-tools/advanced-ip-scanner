// Copyright (c) Famatech Corp. All rights reserved.
// Licensed under the proprietary license. See LICENSE for details.

#include "remote/RdpLauncher.h"

#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QTcpSocket>

namespace aips::remote {

RdpLauncher::RdpLauncher(QObject* parent)
    : QObject(parent)
{
}

bool RdpLauncher::launch(const QHostAddress& address, const RdpConfig& config)
{
    if (!isRdpAvailable()) {
        emit sessionError(address, "mstsc.exe not found");
        return false;
    }

    QStringList args = buildMstscArgs(address, config);

    m_currentProcess = new QProcess(this);
    connect(m_currentProcess, &QProcess::errorOccurred, this, [this, address](QProcess::ProcessError err) {
        emit sessionError(address, QString("RDP process error: %1").arg(err));
    });

    m_currentProcess->start("mstsc.exe", args);

    if (m_currentProcess->waitForStarted(5000)) {
        emit sessionStarted(address);
        return true;
    }

    emit sessionError(address, "Failed to start mstsc.exe");
    return false;
}

bool RdpLauncher::launchWithRdpFile(const QString& rdpFilePath)
{
    if (!QFile::exists(rdpFilePath))
        return false;

    auto* proc = new QProcess(this);
    proc->start("mstsc.exe", {rdpFilePath});
    return proc->waitForStarted(5000);
}

bool RdpLauncher::generateRdpFile(const QHostAddress& address, const RdpConfig& config,
                                    const QString& outputPath)
{
    QFile file(outputPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&file);
    out << "full address:s:" << address.toString();
    if (config.port != 3389)
        out << ":" << config.port;
    out << "\n";

    out << "screen mode id:i:" << (config.fullScreen ? 2 : 1) << "\n";
    out << "desktopwidth:i:" << config.screenWidth << "\n";
    out << "desktopheight:i:" << config.screenHeight << "\n";
    out << "session bpp:i:" << config.colorDepth << "\n";
    out << "use multimon:i:" << (config.multiMonitor ? 1 : 0) << "\n";
    out << "redirectdrives:i:" << (config.redirectDrives ? 1 : 0) << "\n";
    out << "redirectprinters:i:" << (config.redirectPrinters ? 1 : 0) << "\n";
    out << "redirectclipboard:i:" << (config.redirectClipboard ? 1 : 0) << "\n";
    out << "enablecredsspsupport:i:" << (config.enableNLA ? 1 : 0) << "\n";
    out << "authentication level:i:2\n";
    out << "prompt for credentials:i:1\n";
    out << "negotiate security layer:i:1\n";

    if (!config.username.isEmpty())
        out << "username:s:" << config.username << "\n";
    if (!config.domain.isEmpty())
        out << "domain:s:" << config.domain << "\n";
    if (!config.gatewayServer.isEmpty()) {
        out << "gatewayhostname:s:" << config.gatewayServer << "\n";
        out << "gatewayusagemethod:i:1\n";
        out << "gatewayprofileusagemethod:i:1\n";
    }

    return true;
}

bool RdpLauncher::isRdpAvailable()
{
    QString path = QStandardPaths::findExecutable("mstsc");
    if (!path.isEmpty())
        return true;

    return QFile::exists("C:\\Windows\\System32\\mstsc.exe");
}

bool RdpLauncher::isRdpPortOpen(const QHostAddress& address, int timeoutMs)
{
    QTcpSocket socket;
    socket.connectToHost(address, 3389);
    return socket.waitForConnected(timeoutMs);
}

QStringList RdpLauncher::buildMstscArgs(const QHostAddress& address, const RdpConfig& config)
{
    QStringList args;

    QString server = address.toString();
    if (config.port != 3389)
        server += QString(":%1").arg(config.port);

    args << QString("/v:%1").arg(server);

    if (config.fullScreen)
        args << "/f";

    if (config.adminMode)
        args << "/admin";

    if (config.multiMonitor)
        args << "/multimon";

    args << QString("/w:%1").arg(config.screenWidth);
    args << QString("/h:%1").arg(config.screenHeight);

    return args;
}

} // namespace aips::remote
