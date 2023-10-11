#include "server.h"

#include <sputter/log/log.h>
#include <sputter/net/messageprotocol.h>

Server::Server(ClientConnectionCallback connectionCallbackFn, int port)
    : m_listenPort(port), m_connectionCallback(connectionCallbackFn)
{}

bool Server::Listen()
{
    if (m_spListenPort)
    {
        // We're already listening
        return false;
    }

    m_spListenPort.reset(new sputter::net::UDPPort(m_listenPort));
    return m_spListenPort->bind();
}

void Server::Tick()
{
    if (!m_spListenPort || !m_spListenPort->IsBound())
    {
        SetShouldTerminate();
        return;
    }

    std::string address;
    MessageHeader header;
    const int NumReceived = m_spListenPort->receive(&header, sizeof(header), &address);
    if (NumReceived <= 0)
    {
        RELEASE_LOGLINE_VERYVERBOSE(LOG_NET, "No network data received this frame.");
        return;
    }

    if (NumReceived > 0 && NumReceived != sizeof(header))
    {
        RELEASE_LOGLINE_ERROR(
            LOG_NET,
            "Malformed message received. Received %d bytes, expecting %u",
            NumReceived,
            sizeof(header));
        SetShouldTerminate();
        return;
    }

    RELEASE_LOGLINE_INFO(LOG_NET, "New client attempting connection");

    if (header.Type == MessageType::Hello)
    {
        // Return the favor-- hello, new client!
        HelloMessage helloMessage;
        const int sent = m_spListenPort->send(
            &helloMessage, sizeof(helloMessage), address, kDefaultClientPort);
        if (sent != sizeof(helloMessage))
        {
            RELEASE_LOGLINE_ERROR(
                LOG_NET,
                "Failed to send Hello to client at %s",
                address.c_str());
            return;
        }

        RELEASE_LOGLINE_INFO(LOG_NET, "Sent 'Hello' to new client");

        // Register the connection and invoke a callback
        ClientConnection connection { .Address = address };
        m_clientConnections.emplace_back(connection);

        const ClientHandle newClientHandle = m_clientConnections.size();
        if (m_connectionCallback)
        {
            m_connectionCallback(newClientHandle);
        }
        RELEASE_LOGLINE_INFO(LOG_NET, "Added new client (%u)", newClientHandle);
        return;
    }
    else
    {
        RELEASE_LOGLINE_WARNING(
            LOG_NET,
            "Unexpected message type: 0x%08X",
            header.Type);
    }
}

void Server::SetShouldTerminate()
{
    m_shouldTerminate = true;
}

bool Server::ShouldTerminate() const
{
    return m_shouldTerminate;
}

int Server::GetPort() const
{
    return m_listenPort;
}
