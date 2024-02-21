#include "protocol.h"
#include "port.h"
#include "messageprotocol.h"

#include <cstring>
#include <sputter/core/check.h>
#include <sputter/log/log.h>

using namespace sputter;
using namespace sputter::net;

Protocol::Protocol(UDPPortPtr spPort)
    : m_channel(spPort)
{}

void Protocol::Tick()
{
    m_channel.Tick();

    std::string address;
    int port;
    MessageHeader* pMessageHeader = nullptr;
    if (ReceiveNextMessage(&pMessageHeader, &address, &port))
    {
        if (m_messageCallback)
        {
            m_messageCallback(pMessageHeader, address, port);
        }
    }
}

void Protocol::SetMessageReceivedCallback(MessageReceivedCallback callback)
{
    m_messageCallback = callback;
}

bool 
Protocol::SendHelloMessage(
    const std::string& name,
    const std::string* pAddress,
    const int* pPort)
{
    HelloMessage helloMessage;
    if (!CreateHelloMessage(name.c_str(), name.size(), helloMessage))
    {
        RELEASE_LOGLINE_ERROR(LOG_NET, "Failed to create hello message");
        return false;
    }

    const size_t ExpectedSize = helloMessage.Header.MessageSize;
    if (!m_channel.SendChunk(ChunkType::MESSAGE, &helloMessage, ExpectedSize))
    {
        RELEASE_LOGLINE_ERROR(
            LOG_NET,
            "Failed to send Hello message in message chunk.");
        return false;
    }

    return true;
}

bool
Protocol::SendAssignClientIdMessage(
    uint8_t clientId,
    const std::string* pAddress,
    const int* pPort)
{
    AssignClientIdMessage assignClientIdMessage;
    if (!CreateAssignClientIdMessage(clientId, assignClientIdMessage))
    {
        RELEASE_LOGLINE_ERROR(LOG_NET, "Failed to create assign client id message");
        return false;
    }

    const size_t ExpectedSize = assignClientIdMessage.Header.MessageSize;
    if (!m_channel.SendChunk(ChunkType::MESSAGE, &assignClientIdMessage, ExpectedSize))
    {
        RELEASE_LOGLINE_ERROR(
            LOG_NET,
            "Failed to send AssignClientId message in message chunk.");
        return false;
    }

    return true;
}

bool 
Protocol::SendClientReadyMessage(
    uint8_t clientId,
    const std::string* pAddress,
    const int* pPort)
{
    ClientReadyMessage clientReadyMessage;
    if (!CreateClientReadyMessage(clientId, clientReadyMessage))
    {
        RELEASE_LOGLINE_ERROR(LOG_NET, "Failed to create client ready id message");
        return false;
    }

    const size_t ExpectedSize = clientReadyMessage.Header.MessageSize;
    if (!m_channel.SendChunk(ChunkType::MESSAGE, &clientReadyMessage, ExpectedSize))
    {
        RELEASE_LOGLINE_ERROR(
            LOG_NET,
            "Failed to send ClientReadyMessage message in message chunk.");
        return false;
    }

    return true;
}

bool
Protocol::SendStartGameMessage(
    uint32_t gameID,
    const std::string* pAddress,
    const int* pPort)
{
    StartGameMessage startGameMessage;
    if (!CreateStartGameMessage(gameID, startGameMessage))
    {
        RELEASE_LOGLINE_ERROR(LOG_NET, "Failed to create start game message");
        return false;
    }

    const size_t ExpectedSize = startGameMessage.Header.MessageSize;
    if (!m_channel.SendChunk(ChunkType::MESSAGE, &startGameMessage, ExpectedSize))
    {
        RELEASE_LOGLINE_ERROR(
            LOG_NET,
            "Failed to send StartGameMessage message in message chunk.");
        return false;
    }

    return true;
}

bool 
Protocol::ReceiveNextMessage(
    MessageHeader** ppMessageOut,
    std::string* pAddressOut,
    int* pPortOut)
{
    RELEASE_CHECK(ppMessageOut, "Invalid ppMessageOut parameter");

    MessageHeader* pMessage = nullptr;

    size_t messageSize;
    void* pData = nullptr;
    if (!m_channel.ReceiveChunk(ChunkType::MESSAGE, &pData, &messageSize))
    {
        DEBUG_LOGLINE_VERBOSE(
            LOG_NET,
            "No message chunk is available.");
        return false;
    }

    pMessage = static_cast<MessageHeader*>(pData);
    if (pMessage->Type == MessageType::Invalid)
    {
        RELEASE_LOGLINE_WARNING(
            LOG_NET,
            "ReceiveNextMessage() - received invalid message");
        return false;
    }

    if (messageSize != pMessage->MessageSize)
    {
        RELEASE_LOGLINE_WARNING(
            LOG_NET,
            "ReceiveNextMessage() - unexpected size: %d != %d",
            messageSize,
            pMessage->MessageSize);
        return false;
    }

    if (pMessage->Type == MessageType::Hello)
    {
        RELEASE_LOGLINE_INFO(
            LOG_NET,
            "Received 'Hello' message from %s:%d",
            (pAddressOut ? pAddressOut->c_str() : "<null_address>"),
            (pPortOut ? *pPortOut : -1));
        *ppMessageOut = pMessage;
    }
    else if (pMessage->Type == MessageType::ClientReady)
    {
        RELEASE_LOGLINE_INFO(
            LOG_NET,
            "Received 'ClientReady' message from %s:%d",
            (pAddressOut ? pAddressOut->c_str() : "<null_address>"),
            (pPortOut ? *pPortOut : -1));
        *ppMessageOut = pMessage;
    }
    else if (pMessage->Type == MessageType::AssignClientId)
    {
        RELEASE_LOGLINE_INFO(
            LOG_NET,
            "Received 'AssignClientId' message from %s:%d",
            (pAddressOut ? pAddressOut->c_str() : "<null_address>"),
            (pPortOut ? *pPortOut : -1));
        *ppMessageOut = pMessage;
    }
    else if (pMessage->Type == MessageType::StartGame)
    {
        RELEASE_LOGLINE_INFO(
            LOG_NET,
            "Received 'StartGame' message from %s:%d",
            (pAddressOut ? pAddressOut->c_str() : "<null_address>"),
            (pPortOut ? *pPortOut : -1));
        *ppMessageOut = pMessage;
    }
    else if (pMessage->Type == MessageType::Inputs)
    {
        RELEASE_LOGLINE_INFO(
            LOG_NET,
            "Received 'Inputs' message from %s:%d",
            (pAddressOut ? pAddressOut->c_str() : "<null_address>"),
            (pPortOut ? *pPortOut : -1));
        *ppMessageOut = pMessage;
    }
    else
    {
        // TODO: Support other message types
        RELEASE_LOGLINE_WARNING(LOG_NET, "Received unexpected message type");
        return false;
    }

    return true;
}

UDPPortPtr Protocol::GetUDPPort() const
{
    return m_channel.GetUDPPort();
}

