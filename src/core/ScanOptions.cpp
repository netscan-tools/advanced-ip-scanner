// Copyright (c) Famatech Corp. All rights reserved.
// Licensed under the proprietary license. See LICENSE for details.

#include "core/ScanOptions.h"

#include <QJsonArray>

namespace aips::core {

QJsonObject ScanOptions::toJson() const
{
    QJsonObject obj;
    obj["threadCount"]        = threadCount;
    obj["icmpTimeoutMs"]      = icmpTimeoutMs;
    obj["icmpRetries"]        = icmpRetries;
    obj["arpTimeoutMs"]       = arpTimeoutMs;
    obj["dnsTimeoutMs"]       = dnsTimeoutMs;
    obj["netbiosTimeoutMs"]   = netbiosTimeoutMs;
    obj["scanSpeedPercent"]   = scanSpeedPercent;
    obj["useIcmp"]            = useIcmp;
    obj["useArp"]             = useArp;
    obj["resolveHostnames"]   = resolveHostnames;
    obj["resolveNetbios"]     = resolveNetbios;
    obj["detectVendor"]       = detectVendor;
    obj["detectOs"]           = detectOs;
    obj["enumerateShares"]    = enumerateShares;
    obj["checkHttpPorts"]     = checkHttpPorts;
    obj["checkFtpPorts"]      = checkFtpPorts;
    obj["skipOfflineDevices"] = skipOfflineDevices;
    obj["showOnlyAlive"]      = showOnlyAlive;
    obj["favoritesOnly"]      = favoritesOnly;

    QJsonObject portObj;
    portObj["scanCommonPorts"]   = portScan.scanCommonPorts;
    portObj["scanAllPorts"]      = portScan.scanAllPorts;
    portObj["connectTimeoutMs"]  = portScan.connectTimeoutMs;
    portObj["maxConcurrent"]     = portScan.maxConcurrent;

    QJsonArray customPorts;
    for (uint16_t p : portScan.customPorts)
        customPorts.append(static_cast<int>(p));
    portObj["customPorts"] = customPorts;

    obj["portScan"] = portObj;
    return obj;
}

ScanOptions ScanOptions::fromJson(const QJsonObject& obj)
{
    ScanOptions opt;
    opt.threadCount        = obj["threadCount"].toInt(0);
    opt.icmpTimeoutMs      = obj["icmpTimeoutMs"].toInt(500);
    opt.icmpRetries        = obj["icmpRetries"].toInt(2);
    opt.arpTimeoutMs       = obj["arpTimeoutMs"].toInt(300);
    opt.dnsTimeoutMs       = obj["dnsTimeoutMs"].toInt(2000);
    opt.netbiosTimeoutMs   = obj["netbiosTimeoutMs"].toInt(1500);
    opt.scanSpeedPercent   = obj["scanSpeedPercent"].toInt(100);
    opt.useIcmp            = obj["useIcmp"].toBool(true);
    opt.useArp             = obj["useArp"].toBool(true);
    opt.resolveHostnames   = obj["resolveHostnames"].toBool(true);
    opt.resolveNetbios     = obj["resolveNetbios"].toBool(true);
    opt.detectVendor       = obj["detectVendor"].toBool(true);
    opt.detectOs           = obj["detectOs"].toBool(false);
    opt.enumerateShares    = obj["enumerateShares"].toBool(true);
    opt.checkHttpPorts     = obj["checkHttpPorts"].toBool(true);
    opt.checkFtpPorts      = obj["checkFtpPorts"].toBool(true);
    opt.skipOfflineDevices = obj["skipOfflineDevices"].toBool(true);
    opt.showOnlyAlive      = obj["showOnlyAlive"].toBool(false);
    opt.favoritesOnly      = obj["favoritesOnly"].toBool(false);

    QJsonObject portObj = obj["portScan"].toObject();
    opt.portScan.scanCommonPorts  = portObj["scanCommonPorts"].toBool(true);
    opt.portScan.scanAllPorts     = portObj["scanAllPorts"].toBool(false);
    opt.portScan.connectTimeoutMs = portObj["connectTimeoutMs"].toInt(200);
    opt.portScan.maxConcurrent    = portObj["maxConcurrent"].toInt(64);

    const QJsonArray customPorts = portObj["customPorts"].toArray();
    for (const auto& p : customPorts)
        opt.portScan.customPorts.append(static_cast<uint16_t>(p.toInt()));

    return opt;
}

ScanOptions ScanOptions::defaultOptions()
{
    return {};
}

ScanOptions ScanOptions::fastScan()
{
    ScanOptions opt;
    opt.icmpTimeoutMs     = 200;
    opt.icmpRetries       = 1;
    opt.arpTimeoutMs      = 150;
    opt.resolveHostnames  = false;
    opt.resolveNetbios    = false;
    opt.detectVendor      = false;
    opt.enumerateShares   = false;
    opt.portScan.scanCommonPorts = false;
    return opt;
}

ScanOptions ScanOptions::deepScan()
{
    ScanOptions opt;
    opt.icmpTimeoutMs     = 1000;
    opt.icmpRetries       = 3;
    opt.arpTimeoutMs      = 500;
    opt.dnsTimeoutMs      = 5000;
    opt.netbiosTimeoutMs  = 3000;
    opt.detectOs          = true;
    opt.portScan.scanAllPorts = true;
    opt.portScan.connectTimeoutMs = 500;
    return opt;
}

} // namespace aips::core
