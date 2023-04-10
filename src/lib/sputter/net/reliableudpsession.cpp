#include "reliableudpsession.h"
#include "port.h"

#include <sputter/core/check.h>
#include <sputter/system/system.h>
#include <kcp/ikcp.h>

#include <cstring>

#include <vector>
#include <algorithm>

using namespace sputter;
using namespace sputter::net;

struct ReliableUDPSession::PImpl
{
    PImpl(uint32_t sessionId, const std::string& address, int localPort, int remotePortNumber) 
        : SessionId(sessionId), Address(address), Port(localPort), RemotePortNumber(remotePortNumber) {}

    static const uint32_t kInvalidSessionId = 0xFFFFFFFF;
    uint32_t SessionId = kInvalidSessionId;

    std::string Address;
    UDPPort     Port;
    int         RemotePortNumber;

    ikcpcb*  pIkcpCb   = nullptr;
};

ReliableUDPSession::ReliableUDPSession(uint32_t sessionId, const std::string& address, int port, int remotePortNumber)
    : m_spPimpl(new ReliableUDPSession::PImpl(sessionId, address, port, remotePortNumber))
{
    m_spPimpl->pIkcpCb = ikcp_create(sessionId, this);
    RELEASE_CHECK(m_spPimpl->pIkcpCb, "Failed to create KCP object");

    ikcp_setoutput(m_spPimpl->pIkcpCb, SendDataCallback);
    const int kNoDelay = 1;
    const int kUpdateInterval = 1000 / 60; // Once per frame
    const int kEnableFastResend = 1;
    const int kDisableCongestionControl = 1;
    ikcp_nodelay(m_spPimpl->pIkcpCb, kNoDelay, kUpdateInterval, kEnableFastResend, kDisableCongestionControl);
}

ReliableUDPSession::~ReliableUDPSession()
{
    if (m_spPimpl && m_spPimpl->pIkcpCb)
    {
        ikcp_release(m_spPimpl->pIkcpCb);
        m_spPimpl->pIkcpCb = nullptr;
    }
}

void ReliableUDPSession::Tick()
{
    // Process any received data input before updating
    // Size is arbitrary for the time being
    char buffer[2048] = {};

    int received = m_spPimpl->Port.receive(buffer, sizeof(buffer));
    while (received > 0)
    {
        ikcp_input(m_spPimpl->pIkcpCb, buffer, received);
        received = m_spPimpl->Port.receive(buffer, sizeof(buffer));
    }

    ikcp_update(m_spPimpl->pIkcpCb, system::GetTimeMs());
}

size_t ReliableUDPSession::EnqueueSendData(const char* pBuffer, size_t length)
{
    if (ikcp_send(m_spPimpl->pIkcpCb, pBuffer, length) == 0)
    {
        return length;
    }

    return 0;
}

size_t ReliableUDPSession::TryReadData(char* pBuffer, size_t maxLength)
{
    const int ReturnValue = ikcp_recv(m_spPimpl->pIkcpCb, pBuffer, maxLength);
    if (ReturnValue < 0)
    {
        return 0;
    }

    return static_cast<size_t>(ReturnValue);
}

int ReliableUDPSession::SendDataCallback(const char* pBuffer, int length, struct IKCPCB* pKcp, void* pUser)
{
    auto pSession = static_cast<ReliableUDPSession*>(pUser);
    RELEASE_CHECK(pKcp == pSession->m_spPimpl->pIkcpCb, "KCP pointer mismatch");

    const UDPPort& Port = pSession->m_spPimpl->Port;
    const std::string& Address = pSession->m_spPimpl->Address;
    const int RemotePort = pSession->m_spPimpl->RemotePortNumber;
    return Port.send(pBuffer, length, Address, RemotePort);
}

int ReliableUDPSession::GetPort() const
{
    return m_spPimpl->Port.GetPort();
}