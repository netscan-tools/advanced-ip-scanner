// Copyright (c) Famatech Corp. All rights reserved.
// Licensed under the proprietary license. See LICENSE for details.

#include "net/IcmpProbe.h"

#include <QElapsedTimer>
#include <memory>
#include <cstring>

#ifdef Q_OS_WIN
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")
#endif

namespace aips::net {

IcmpProbe::IcmpProbe()
{
#ifdef Q_OS_WIN
    m_hIcmp = IcmpCreateFile();
    m_available = (m_hIcmp != INVALID_HANDLE_VALUE);
#endif
}

IcmpProbe::~IcmpProbe()
{
#ifdef Q_OS_WIN
    if (m_hIcmp != INVALID_HANDLE_VALUE)
        IcmpCloseHandle(m_hIcmp);
#endif
}

PingResult IcmpProbe::ping(const QHostAddress& address, int timeoutMs, int payloadSize)
{
    PingResult result;
    ++m_pingsSent;

#ifdef Q_OS_WIN
    if (!m_available)
        return result;

    auto sendData = std::make_unique<char[]>(payloadSize);
    std::memset(sendData.get(), 0xAA, payloadSize);

    DWORD replySize = sizeof(ICMP_ECHO_REPLY) + payloadSize + 8;
    auto replyBuf   = std::make_unique<BYTE[]>(replySize);

    QElapsedTimer timer;
    timer.start();

    DWORD ret = IcmpSendEcho(
        m_hIcmp,
        htonl(address.toIPv4Address()),
        sendData.get(),
        static_cast<WORD>(payloadSize),
        nullptr,
        replyBuf.get(),
        replySize,
        static_cast<DWORD>(timeoutMs)
    );

    if (ret > 0) {
        auto* reply = reinterpret_cast<PICMP_ECHO_REPLY>(replyBuf.get());
        if (reply->Status == IP_SUCCESS) {
            result.success    = true;
            result.latencyMs  = static_cast<double>(reply->RoundTripTime);
            result.ttl        = reply->Options.Ttl;
            result.replySize  = static_cast<int>(reply->DataSize);
            ++m_pingsReceived;
        }
        result.statusCode = static_cast<uint8_t>(reply->Status);
    }
#else
    Q_UNUSED(address);
    Q_UNUSED(timeoutMs);
    Q_UNUSED(payloadSize);
#endif

    return result;
}

PingResult IcmpProbe::pingWithRetry(const QHostAddress& address, int timeoutMs, int retries)
{
    PingResult best;

    for (int i = 0; i < retries; ++i) {
        PingResult result = ping(address, timeoutMs);
        if (result.success) {
            if (!best.success || result.latencyMs < best.latencyMs)
                best = result;
            break;
        }
    }

    return best;
}

} // namespace aips::net
