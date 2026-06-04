// Copyright (c) Famatech Corp. All rights reserved.
// Licensed under the proprietary license. See LICENSE for details.

#include "ui/MainWindow.h"
#include "utils/AppConfig.h"
#include "utils/Logger.h"
#include "net/VendorDb.h"

#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QSplashScreen>
#include <QPixmap>
#include <QTimer>
#include <QCommandLineParser>
#include <QStyleFactory>

#ifdef Q_OS_WIN
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#endif

namespace {

void initializeWinsock()
{
#ifdef Q_OS_WIN
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        qFatal("WSAStartup failed with error: %d", result);
    }
#endif
}

void cleanupWinsock()
{
#ifdef Q_OS_WIN
    WSACleanup();
#endif
}

struct WinsockGuard {
    WinsockGuard()  { initializeWinsock(); }
    ~WinsockGuard() { cleanupWinsock(); }
};

} // anonymous namespace

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("Advanced IP Scanner"));
    app.setApplicationVersion(QStringLiteral(AIPS_VERSION));
    app.setOrganizationName(QStringLiteral("Famatech"));
    app.setOrganizationDomain(QStringLiteral("famatech.com"));

    WinsockGuard winsockGuard;

    QCommandLineParser parser;
    parser.setApplicationDescription("Advanced IP Scanner — Fast and reliable network scanner");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption rangeOption(
        {"r", "range"},
        "IP range to scan on startup (e.g. 192.168.1.0/24)",
        "range"
    );
    parser.addOption(rangeOption);

    QCommandLineOption portableOption(
        {"p", "portable"},
        "Run in portable mode"
    );
    parser.addOption(portableOption);

    QCommandLineOption silentOption(
        {"s", "silent"},
        "Start minimized to tray"
    );
    parser.addOption(silentOption);

    QCommandLineOption logOption(
        {"l", "log"},
        "Log file path",
        "file"
    );
    parser.addOption(logOption);

    parser.process(app);

    // Initialize logger
    auto& logger = aips::utils::Logger::instance();
    QString logPath = parser.isSet(logOption)
                      ? parser.value(logOption)
                      : aips::utils::AppConfig::instance().dataDir() + "/scanner.log";
    logger.init(logPath);
    AIPS_LOG_INFO("App", QString("Advanced IP Scanner %1 starting").arg(app.applicationVersion()));

    // Load configuration
    auto& config = aips::utils::AppConfig::instance();
    config.load();

    // Load OUI vendor database
    aips::net::VendorDb::instance().loadFromResource();
    AIPS_LOG_INFO("App", QString("Vendor database: %1 entries")
                  .arg(aips::net::VendorDb::instance().entryCount()));

    // Load translations
    QTranslator translator;
    QString locale = config.language();
    if (translator.load("aips_" + locale, ":/translations"))
        app.installTranslator(&translator);

    // Apply system style
    app.setStyle(QStyleFactory::create("Fusion"));

    // Show splash screen
    QPixmap splashPixmap(":/images/splash.png");
    QSplashScreen splash(splashPixmap);
    splash.show();
    splash.showMessage("Loading...", Qt::AlignBottom | Qt::AlignHCenter, Qt::white);
    app.processEvents();

    // Create main window
    aips::ui::MainWindow mainWindow;

    QTimer::singleShot(1500, &splash, [&splash, &mainWindow]() {
        splash.finish(&mainWindow);
    });

    if (parser.isSet(silentOption) || config.startMinimized()) {
        mainWindow.hide();
    } else {
        mainWindow.show();
    }

    AIPS_LOG_INFO("App", "Application initialized successfully");

    int exitCode = app.exec();

    config.save();
    logger.shutdown();

    return exitCode;
}
