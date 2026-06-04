// Copyright (c) Famatech Corp. All rights reserved.
// Licensed under the proprietary license. See LICENSE for details.

#pragma once

#include <QObject>
#include <QHostAddress>
#include <QString>
#include <QProcess>

namespace aips::remote {

struct RdpConfig {
    QString  username;
    QString  domain;
    int      port           = 3389;
    int      screenWidth    = 1920;
    int      screenHeight   = 1080;
    int      colorDepth     = 32;
    bool     fullScreen     = false;
    bool     adminMode      = false;
    bool     multiMonitor   = false;
    bool     redirectDrives = false;
    bool     redirectPrinters = false;
    bool     redirectClipboard = true;
    bool     enableNLA      = true;
    QString  gatewayServer;
};

class RdpLauncher : public QObject {
    Q_OBJECT

public:
    explicit RdpLauncher(QObject* parent = nullptr);
    ~RdpLauncher() override = default;

    bool launch(const QHostAddress& address, const RdpConfig& config = {});
    bool launchWithRdpFile(const QString& rdpFilePath);

    bool generateRdpFile(const QHostAddress& address, const RdpConfig& config,
                          const QString& outputPath);

    [[nodiscard]] static bool isRdpAvailable();
    [[nodiscard]] static bool isRdpPortOpen(const QHostAddress& address, int timeoutMs = 1000);

signals:
    void sessionStarted(const QHostAddress& address);
    void sessionError(const QHostAddress& address, const QString& error);

private:
    QStringList buildMstscArgs(const QHostAddress& address, const RdpConfig& config);
    QProcess*   m_currentProcess = nullptr;
};

} // namespace aips::remote
