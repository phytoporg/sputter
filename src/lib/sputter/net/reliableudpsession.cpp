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

    std::vector<uint8_t> SendBuffer;
    std::vector<uint8_t> ReadBuffer;
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
    RELEASE_CHECK(m_spPimpl && m_spPimpl->pIkcpCb, "KCP state is uninitialized");
    ikcp_update(m_spPimpl->pIkcpCb, system::GetTimeMs());
}

void ReliableUDPSession::EnqueueSendData(const char* pBuffer, size_t length)
{
    m_spPimpl->SendBuffer.insert(std::end(m_spPimpl->SendBuffer), pBuffer, pBuffer + length);
}

size_t ReliableUDPSession::TryReadData(char* pBuffer, size_t maxLength)
{
    const size_t DataSize = m_spPimpl->ReadBuffer.size();
    const size_t BytesToCopy = maxLength < DataSize ? maxLength : DataSize;

    if (!BytesToCopy)
    {
        return 0;
    }

    auto& readBuffer = m_spPimpl->ReadBuffer;
    RELEASE_CHECK(readBuffer.size() >= BytesToCopy, "Insufficient space in read buffer");

    memcpy(pBuffer, readBuffer.data(), BytesToCopy);
    readBuffer.erase(std::begin(readBuffer), std::begin(readBuffer) + BytesToCopy);

    return BytesToCopy;
}