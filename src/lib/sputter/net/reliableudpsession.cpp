#include "reliableudpsession.h"
#include "port.h"

#include <sputter/core/check.h>
#include <sputter/system/system.h>

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
};

ReliableUDPSession::ReliableUDPSession(uint32_t sessionId, int localPort, const std::string& address, int remotePortNumber)
    : m_spPimpl(new ReliableUDPSession::PImpl(sessionId, localPort, address, remotePortNumber))
{
    RELEASE_CHECK(m_spPimpl->Port.bind(), "Failed to bind to local port");
    RELEASE_CHECK(m_spPimpl->Port.connect(address, remotePortNumber), "Reliable UDP session failed to connect");
}

ReliableUDPSession::ReliableUDPSession(uint32_t sessionId, const UDPPort& port, const std::string& address, int remotePortNumber)
    : m_spPimpl(new ReliableUDPSession::PImpl(sessionId, port, address, remotePortNumber))
{
    m_spPimpl->Port.bind();
    m_spPimpl->Port.connect(address, remotePortNumber);
}

ReliableUDPSession::ReliableUDPSession(uint32_t sessionId, const UDPPort& port)
    : m_spPimpl(new ReliableUDPSession::PImpl(sessionId, port, port.GetRemoteAddress(), port.GetRemotePort()))
{
    // In this case, the port should already be bound and connected
}

ReliableUDPSession::~ReliableUDPSession() {}

void ReliableUDPSession::Tick()
{
    // NOOP for now
}

size_t ReliableUDPSession::Send(const char *pBuffer, size_t length)
{
    return m_spPimpl->Port.send(
        pBuffer,
        length,
        &m_spPimpl->Address,
        &m_spPimpl->RemotePortNumber);
}

size_t ReliableUDPSession::SendReliable(const char* pBuffer, size_t length)
{
    // TODO: Reliable implementation
    return Send(pBuffer, length);
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
    // TODO: Read reliably
    return Read(pBuffer, maxLength);
}

size_t ReliableUDPSession::PeekSize() const
{
    // TODO: Kill this function
    return 0;
}

int ReliableUDPSession::GetPort() const
{
    return m_spPimpl->Port.GetPort();
}

void net::ReliableUDPSession::Flush()
{
    // NOOP
}
