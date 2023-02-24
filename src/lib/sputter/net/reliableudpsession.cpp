#include "reliableudpsession.h"
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
    PImpl(uint32_t sessionId) : SessionId(sessionId) {}

    static const uint32_t kInvalidSessionId = 0xFFFFFFFF;
    uint32_t SessionId = kInvalidSessionId;
    ikcpcb*  pIkcpCb   = nullptr;
};

ReliableUDPSession::ReliableUDPSession(uint32_t sessionId)
    : m_spPimpl(new ReliableUDPSession::PImpl(sessionId))
{
    m_spPimpl->pIkcpCb = ikcp_create(sessionId, this);
    RELEASE_CHECK(m_spPimpl->pIkcpCb, "Failed to create KCP object");
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

void ReliableUDPSession::SendDataCallback(const char* pBuffer, int length)
{
    // TODO: Sockets
}