// Copyright (c) Famatech Corp. All rights reserved.
// Licensed under the proprietary license. See LICENSE for details.

#pragma once

#include <QVector>
#include <QJsonObject>

#include <cstdint>

namespace aips::core {

struct PortScanConfig {
    QVector<uint16_t> customPorts;
    bool              scanCommonPorts  = true;
    bool              scanAllPorts     = false;
    int               connectTimeoutMs = 200;
    int               maxConcurrent    = 64;
};

struct ScanOptions {
    int  threadCount         = 0;       // 0 = auto-detect
    int  icmpTimeoutMs       = 500;
    int  icmpRetries         = 2;
    int  arpTimeoutMs        = 300;
    int  dnsTimeoutMs        = 2000;
    int  netbiosTimeoutMs    = 1500;
    int  scanSpeedPercent    = 100;      // throttle: 1-100
    bool useIcmp             = true;
    bool useArp              = true;
    bool resolveHostnames    = true;
    bool resolveNetbios      = true;
    bool detectVendor        = true;
    bool detectOs            = false;
    bool enumerateShares     = true;
    bool checkHttpPorts      = true;
    bool checkFtpPorts       = true;
    bool skipOfflineDevices  = true;
    bool showOnlyAlive       = false;
    bool favoritesOnly       = false;

    PortScanConfig portScan;

    [[nodiscard]] QJsonObject toJson() const;
    [[nodiscard]] static ScanOptions fromJson(const QJsonObject& obj);
    [[nodiscard]] static ScanOptions defaultOptions();
    [[nodiscard]] static ScanOptions fastScan();
    [[nodiscard]] static ScanOptions deepScan();
};

} // namespace aips::core
