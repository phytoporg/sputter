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
    PImpl(uint32_t sessionId, const std::string& address, int port) 
        : SessionId(sessionId), Address(address), Port(port) {}

    static const uint32_t kInvalidSessionId = 0xFFFFFFFF;
    uint32_t SessionId = kInvalidSessionId;

    std::string Address;
    UDPPort     Port;

    ikcpcb*  pIkcpCb   = nullptr;
};

ReliableUDPSession::ReliableUDPSession(uint32_t sessionId, const std::string& address, int port)
    : m_spPimpl(new ReliableUDPSession::PImpl(sessionId, address, port))
{
    m_spPimpl->pIkcpCb = ikcp_create(sessionId, this);
    RELEASE_CHECK(m_spPimpl->pIkcpCb, "Failed to create KCP object");

    ikcp_setoutput(m_spPimpl->pIkcpCb, SendDataCallback);
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
    ikcp_update(m_spPimpl->pIkcpCb, system::GetTimeMs());
}

size_t ReliableUDPSession::EnqueueSendData(const char* pBuffer, size_t length)
{
    return ikcp_send(m_spPimpl->pIkcpCb, pBuffer, length);
}

size_t ReliableUDPSession::TryReadData(char* pBuffer, size_t maxLength)
{
    return ikcp_recv(m_spPimpl->pIkcpCb, pBuffer, maxLength);
}

int ReliableUDPSession::SendDataCallback(const char* pBuffer, int length, struct IKCPCB* pKcp, void* pUser)
{
    auto pSession = static_cast<ReliableUDPSession*>(pUser);
    RELEASE_CHECK(pKcp == pSession->m_spPimpl->pIkcpCb, "KCP pointer mismatch");
    // TODO
    return 0;
}