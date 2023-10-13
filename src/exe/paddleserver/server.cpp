#include "server.h"

#include <cstring>
#include <sputter/log/log.h>

Server::Server(ClientConnectionCallback connectionCallbackFn, int port)
    : m_listenPort(port), m_connectionCallback(connectionCallbackFn)
{}

bool Server::Listen()
{
    if (m_spListenPort && m_spListenPort->IsBound() && m_spProtocol)
    {
        // We're already listening
        return false;
    }

    if (!m_spListenPort)
    {
        m_spListenPort.reset(new sputter::net::UDPPort(m_listenPort));
    }

    // TODO: IsBound() doesn't do what I expect, fix this
    //if (!m_spListenPort->IsBound())
    {
        if (!m_spListenPort->bind())
        {
            return false;
        }
    }

    m_spProtocol.reset(new sputter::net::Protocol(m_spListenPort));
    return m_spProtocol != nullptr && m_spListenPort->IsBound();
}

void Server::Tick()
{
    if (!m_spListenPort || !m_spListenPort->IsBound())
    {
        SetShouldTerminate();
        return;
    }

    ReceiveMessages();
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

bool Server::GetClientAddress(ClientHandle handle, std::string& addressOut) const
{
    if (handle < 0 || handle > m_clientConnections.size())
    {
        return false;
    }

    addressOut = m_clientConnections[handle - 1].Address;
    return true;
}

bool Server::GetClientPort(ClientHandle handle, int& portOut) const
{
    if (handle < 0 || handle > m_clientConnections.size())
    {
        return false;
    }

    portOut = m_clientConnections[handle - 1].Port;
    return true;
}

void Server::ReceiveMessages()
{
    std::string receivingAddress;
    int receivingPort;
    MessageHeader* pMessage = nullptr;
    const bool Success = 
        m_spProtocol->ReceiveNextMessage(
            &pMessage,
            &receivingAddress,
            &receivingPort);
    if (!Success)
    {
        return;
    }

    RELEASE_LOGLINE_INFO(LOG_NET, "New client attempting connection");

    if (pMessage->Type == MessageType::Hello)
    {
        auto pHelloMessage = reinterpret_cast<HelloMessage*>(pMessage);
        HandleReceiveHello(pHelloMessage, receivingAddress, receivingPort);
        delete pHelloMessage;
    }
    else
    {
        RELEASE_LOGLINE_WARNING(
            LOG_NET,
            "Unexpected message type: 0x%08X",
            pMessage->Type);
    }
}

bool 
Server::HandleReceiveHello(
    HelloMessage* pHelloMessage,
    const std::string& address,
    int port)
{
    if (pHelloMessage->Header.MessageSize < sizeof(MessageHeader))
    {
        RELEASE_LOGLINE_WARNING(
            LOG_NET,
            "Hello message has unexpected size: %d",
            pHelloMessage->Header.MessageSize);
        return false;
    }

    // Are we already connected to this client?
    const std::string ClientName(pHelloMessage->Name, pHelloMessage->NameSize);
    if (FindClient(ClientName, address, port))
    {
        RELEASE_LOGLINE_INFO(
            LOG_NET,
            "Received 'Hello' from alread-connected client: %s @ %s:%d",
            ClientName.c_str(),
            address.c_str(),
            port);
        return false;
    }

    if (pHelloMessage->NameSize == 0)
    {
        RELEASE_LOGLINE_WARNING(
            LOG_NET,
            "Received client 'Hello' message has no name",
            pHelloMessage->Header.MessageSize);
        return false;
    }

    const size_t ExpectedSize = 
        HelloMessage::GetExpectedSize(pHelloMessage->Name, pHelloMessage->NameSize);
    if (pHelloMessage->Header.MessageSize != ExpectedSize)
    {
        RELEASE_LOGLINE_WARNING(
            LOG_NET,
            "Unexpected 'Hello' message size: %u != %u (name = %s)",
            pHelloMessage->Header.MessageSize,
            ExpectedSize, 
            pHelloMessage->Name);
        return false;
    }

    // Return the favor-- hello, new client!
    HelloMessage helloMessage;
    const char* pServerName = "Server";
    if (!CreateHelloMessage(pServerName, strlen(pServerName), helloMessage))
    {
        RELEASE_LOGLINE_ERROR(
            LOG_NET,
            "Failed to create hello message to send to new client");
        return false;
    }

    const int sent = m_spListenPort->send(
        &helloMessage, sizeof(helloMessage), address, kDefaultClientPort);
    if (sent != sizeof(helloMessage))
    {
        RELEASE_LOGLINE_ERROR(
            LOG_NET,
            "Failed to send Hello to client at %s:%d",
            address.c_str(),
            port);
        return false;
    }

    RELEASE_LOGLINE_INFO(LOG_NET, "Sent 'Hello' to new client");

    // Register the connection and invoke a callback
    ClientConnection connection {
        .Name = std::string(pHelloMessage->Name, pHelloMessage->NameSize),
        .Address = address,
        .Port = port };
    m_clientConnections.emplace_back(connection);

    const ClientHandle newClientHandle = m_clientConnections.size();
    if (m_connectionCallback)
    {
        m_connectionCallback(this, newClientHandle);
    }
    RELEASE_LOGLINE_INFO(LOG_NET, "Added new client (%u)", newClientHandle);
    return true;
}

bool 
Server::FindClient(
    const std::string& name,
    const std::string& address,
    int port)
{
    for (const ClientConnection& connection : m_clientConnections)
    {
        if (connection.Name == name && 
            connection.Address == address && 
            port == connection.Port)
        {
            return true;
        }
    }

    return false;
}
