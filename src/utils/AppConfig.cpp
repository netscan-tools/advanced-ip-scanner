// Copyright (c) Famatech Corp. All rights reserved.
// Licensed under the proprietary license. See LICENSE for details.

#include "utils/AppConfig.h"

#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>

namespace aips::utils {

AppConfig& AppConfig::instance()
{
    static AppConfig config;
    return config;
}

AppConfig::AppConfig() = default;

void AppConfig::load()
{
    auto settings = createSettings();

    m_language       = settings->value("General/Language", "en").toString();
    m_minimizeToTray = settings->value("General/MinimizeToTray", false).toBool();
    m_startMinimized = settings->value("General/StartMinimized", false).toBool();
    m_checkUpdates   = settings->value("General/CheckUpdates", true).toBool();
    m_lastRange      = settings->value("Scan/LastRange", "192.168.1.1-254").toString();

    QJsonObject scanObj;
    scanObj["threadCount"]      = settings->value("Scan/ThreadCount", 0).toInt();
    scanObj["icmpTimeoutMs"]    = settings->value("Scan/IcmpTimeout", 500).toInt();
    scanObj["icmpRetries"]      = settings->value("Scan/IcmpRetries", 2).toInt();
    scanObj["arpTimeoutMs"]     = settings->value("Scan/ArpTimeout", 300).toInt();
    scanObj["dnsTimeoutMs"]     = settings->value("Scan/DnsTimeout", 2000).toInt();
    scanObj["netbiosTimeoutMs"] = settings->value("Scan/NetbiosTimeout", 1500).toInt();
    scanObj["scanSpeedPercent"] = settings->value("Scan/Speed", 100).toInt();
    scanObj["useIcmp"]          = settings->value("Scan/UseIcmp", true).toBool();
    scanObj["useArp"]           = settings->value("Scan/UseArp", true).toBool();
    scanObj["resolveHostnames"] = settings->value("Scan/ResolveHostnames", true).toBool();
    scanObj["resolveNetbios"]   = settings->value("Scan/ResolveNetbios", true).toBool();
    scanObj["detectVendor"]     = settings->value("Scan/DetectVendor", true).toBool();
    scanObj["detectOs"]         = settings->value("Scan/DetectOs", false).toBool();
    scanObj["enumerateShares"]  = settings->value("Scan/EnumerateShares", true).toBool();

    m_scanOptions = aips::core::ScanOptions::fromJson(scanObj);
}

void AppConfig::save()
{
    auto settings = createSettings();

    settings->setValue("General/Language",       m_language);
    settings->setValue("General/MinimizeToTray", m_minimizeToTray);
    settings->setValue("General/StartMinimized", m_startMinimized);
    settings->setValue("General/CheckUpdates",   m_checkUpdates);
    settings->setValue("Scan/LastRange",         m_lastRange);

    auto obj = m_scanOptions.toJson();
    settings->setValue("Scan/ThreadCount",       obj["threadCount"].toInt());
    settings->setValue("Scan/IcmpTimeout",       obj["icmpTimeoutMs"].toInt());
    settings->setValue("Scan/IcmpRetries",       obj["icmpRetries"].toInt());
    settings->setValue("Scan/ArpTimeout",        obj["arpTimeoutMs"].toInt());
    settings->setValue("Scan/DnsTimeout",        obj["dnsTimeoutMs"].toInt());
    settings->setValue("Scan/NetbiosTimeout",    obj["netbiosTimeoutMs"].toInt());
    settings->setValue("Scan/Speed",             obj["scanSpeedPercent"].toInt());
    settings->setValue("Scan/UseIcmp",           obj["useIcmp"].toBool());
    settings->setValue("Scan/UseArp",            obj["useArp"].toBool());
    settings->setValue("Scan/ResolveHostnames",  obj["resolveHostnames"].toBool());
    settings->setValue("Scan/ResolveNetbios",    obj["resolveNetbios"].toBool());
    settings->setValue("Scan/DetectVendor",      obj["detectVendor"].toBool());
    settings->setValue("Scan/DetectOs",          obj["detectOs"].toBool());
    settings->setValue("Scan/EnumerateShares",   obj["enumerateShares"].toBool());

    settings->sync();
}

void AppConfig::resetToDefaults()
{
    m_language       = "en";
    m_minimizeToTray = false;
    m_startMinimized = false;
    m_checkUpdates   = true;
    m_lastRange      = "192.168.1.1-254";
    m_scanOptions    = aips::core::ScanOptions::defaultOptions();
}

QString AppConfig::configFilePath() const
{
    if (isPortable())
        return QCoreApplication::applicationDirPath() + "/config.ini";

    return QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/config.ini";
}

QString AppConfig::dataDir() const
{
    if (isPortable())
        return QCoreApplication::applicationDirPath() + "/data";

    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
}

QString AppConfig::favoritesFilePath() const
{
    return dataDir() + "/favorites.json";
}

bool AppConfig::isPortable() const
{
#ifdef AIPS_PORTABLE
    return true;
#else
    return QFile::exists(QCoreApplication::applicationDirPath() + "/portable.dat");
#endif
}

std::unique_ptr<QSettings> AppConfig::createSettings() const
{
    if (isPortable()) {
        return std::make_unique<QSettings>(configFilePath(), QSettings::IniFormat);
    }
    return std::make_unique<QSettings>("Famatech", "AdvancedIPScanner");
}

} // namespace aips::utils
