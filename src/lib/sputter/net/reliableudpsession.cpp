#include "reliableudpsession.h"
#include "port.h"

#include <sputter/core/check.h>
#include <sputter/system/system.h>
#include <kcp/ikcp.h>

#include <vector>
#include <netinet/in.h>

using namespace sputter;
using namespace sputter::net;

struct ReliableUDPSession::PImpl
{
    PImpl(uint32_t sessionId, int localPort, const std::string& address, int remotePortNumber)
        : SessionId(sessionId), Port(localPort), Address(address), RemotePortNumber(remotePortNumber) {}

    PImpl(uint32_t sessionId, const UDPPort& port, const std::string& address, int remotePortNumber)
        : SessionId(sessionId), Port(port), Address(address), RemotePortNumber(remotePortNumber) {}

    static const uint32_t kInvalidSessionId = 0xFFFFFFFF;
    uint32_t SessionId = kInvalidSessionId;

    std::string Address;
    UDPPort     Port;
    int         RemotePortNumber = -1;
    ikcpcb*     pIkcpCb = nullptr;
};

ReliableUDPSession::ReliableUDPSession(uint32_t sessionId, int localPort, const std::string& address, int remotePortNumber)
    : m_spPimpl(new ReliableUDPSession::PImpl(sessionId, localPort, address, remotePortNumber))
{
    RELEASE_CHECK(m_spPimpl->Port.bind(), "Failed to bind to local port");
    RELEASE_CHECK(m_spPimpl->Port.connect(address, remotePortNumber), "Reliable UDP session failed to connect");

    m_spPimpl->pIkcpCb = ikcp_create(sessionId, this);
    RELEASE_CHECK(m_spPimpl->pIkcpCb, "Failed to create KCP object");

    ikcp_setoutput(m_spPimpl->pIkcpCb, SendDataCallback);
    const int kNoDelay = 1;
    const int kUpdateInterval = 1000 / 60; // Once per frame
    const int kEnableFastResend = 1;
    const int kDisableCongestionControl = 1;
    ikcp_nodelay(m_spPimpl->pIkcpCb, kNoDelay, kUpdateInterval, kEnableFastResend, kDisableCongestionControl);
}

ReliableUDPSession::ReliableUDPSession(uint32_t sessionId, const UDPPort& port, const std::string& address, int remotePortNumber)
    : m_spPimpl(new ReliableUDPSession::PImpl(sessionId, port, address, remotePortNumber))
{
    m_spPimpl->Port.bind();
    m_spPimpl->Port.connect(address, remotePortNumber);

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

size_t ReliableUDPSession::Send(const char *pBuffer, size_t length)
{
    return m_spPimpl->Port.send(
        pBuffer,
        length,
        m_spPimpl->Address,
        m_spPimpl->RemotePortNumber);
}

size_t ReliableUDPSession::SendReliable(const char* pBuffer, size_t length)
{
    if (ikcp_send(m_spPimpl->pIkcpCb, pBuffer, length) == 0)
    {
        return length;
    }

    return 0;
}

size_t ReliableUDPSession::Read(char* pBuffer, size_t maxLength)
{
    std::string sourceAddress;
    const size_t BytesRead =
        m_spPimpl->Port.receive(
            pBuffer,
            maxLength,
            &sourceAddress);
    if (sourceAddress != m_spPimpl->Address)
    {
        return -1;
    }

    return BytesRead;
}

size_t ReliableUDPSession::ReadReliable(char* pBuffer, size_t maxLength)
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

size_t net::ReliableUDPSession::PeekSize() const
{
    const int Size = ikcp_peeksize(m_spPimpl->pIkcpCb);
    return Size < 0 ? 0 : Size;
}

void net::ReliableUDPSession::Flush()
{
    ikcp_flush(m_spPimpl->pIkcpCb);
}
