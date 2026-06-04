// Copyright (c) Famatech Corp. All rights reserved.
// Licensed under the proprietary license. See LICENSE for details.

#pragma once

#include <QHostAddress>
#include <cstdint>

#ifdef Q_OS_WIN
#include <winsock2.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#endif

namespace aips::net {

struct PingResult {
    bool    success    = false;
    double  latencyMs  = 0.0;
    int     ttl        = 0;
    int     replySize  = 0;
    uint8_t statusCode = 0;
};

class IcmpProbe {
public:
    IcmpProbe();
    ~IcmpProbe();

    IcmpProbe(const IcmpProbe&) = delete;
    IcmpProbe& operator=(const IcmpProbe&) = delete;

    PingResult ping(const QHostAddress& address, int timeoutMs = 500, int payloadSize = 32);
    PingResult pingWithRetry(const QHostAddress& address, int timeoutMs, int retries);

    [[nodiscard]] bool isAvailable() const noexcept { return m_available; }
    [[nodiscard]] uint64_t totalPingsSent()     const noexcept { return m_pingsSent; }
    [[nodiscard]] uint64_t totalPingsReceived() const noexcept { return m_pingsReceived; }

private:
    bool     m_available = false;
    uint64_t m_pingsSent     = 0;
    uint64_t m_pingsReceived = 0;

#ifdef Q_OS_WIN
    HANDLE m_hIcmp = INVALID_HANDLE_VALUE;
#endif
};

} // namespace aips::net
