#include "protocol.h"
#include "port.h"
#include "messageprotocol.h"

#include <cstring>
#include <sputter/core/check.h>
#include <sputter/log/log.h>

using namespace sputter;
using namespace sputter::net;

Protocol::Protocol(UDPPortPtr spPort)
    : m_spPort(spPort),
      m_messagePool(32)
{
    RELEASE_CHECK(m_spPort != nullptr, "Invalid port provided to protocol");
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
    const int sent = m_spPort->send(&helloMessage, ExpectedSize, pAddress, pPort);
    if (sent != ExpectedSize)
    {
        RELEASE_LOGLINE_ERROR(
            LOG_NET,
            "Failed to send Hello message. Sent %u, not %u",
            sent, ExpectedSize);
        return false;
    }

    RELEASE_LOGLINE_INFO(LOG_NET, "Sent Hello message, size = %u", sent);
    return true;
}

bool 
Protocol::ReceiveHelloMessage(
    HelloMessage* pHelloMessageOut,
    std::string* pAddressOut,
    int* pPortOut)
{
    RELEASE_CHECK(pHelloMessageOut, "Invalid pHelloMessageOut parameter");
    int numReceived = 
        m_spPort->receive(
            pHelloMessageOut,
            sizeof(HelloMessage),
            pAddressOut,
            pPortOut);
    if (numReceived <= 0)
    {
        return false;
    }

    if (numReceived != pHelloMessageOut->Header.MessageSize)
    {
        RELEASE_LOGLINE_WARNING(
            LOG_NET,
            "ReceiveHelloMessage() - unexpected size: %d != %d",
            numReceived,
            pHelloMessageOut->Header.MessageSize);
        return false;
    }

    if (pHelloMessageOut->Header.Type != MessageType::Hello)
    {
        RELEASE_LOGLINE_WARNING(
            LOG_NET,
            "ReceiveHelloMessage() - unexpected message type");
        return false;
    }


    RELEASE_LOGLINE_INFO(
        LOG_NET,
        "Received 'Hello' message from %s:%d",
        (pAddressOut ? pAddressOut->c_str() : "<null_address>"),
        (pPortOut ? *pPortOut : -1));
     
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
    const int sent = 
        m_spPort->send(&assignClientIdMessage, ExpectedSize, pAddress, pPort);
    if (sent != ExpectedSize)
    {
        RELEASE_LOGLINE_ERROR(
            LOG_NET,
            "Failed to send AssignClientId message. Sent %u, not %u",
            sent, ExpectedSize);
        return false;
    }

    RELEASE_LOGLINE_INFO(LOG_NET, "Sent AssignClientId message, size = %u", sent);
    return true;
}

bool 
Protocol::ReceiveAssignClientIdMessage(
    AssignClientIdMessage* pAssignClientIdMessageOut,
    std::string* pAddressOut,
    int* pPortOut)
{
    RELEASE_CHECK(
        pAssignClientIdMessageOut,
        "Invalid pAssignClientIdMessageOut parameter");
    int numReceived = 
        m_spPort->receive(
            pAssignClientIdMessageOut,
            sizeof(AssignClientIdMessage),
            pAddressOut,
            pPortOut);
    if (numReceived <= 0)
    {
        return false;
    }

    if (numReceived != pAssignClientIdMessageOut->Header.MessageSize)
    {
        RELEASE_LOGLINE_WARNING(
            LOG_NET,
            "ReceiveAssignClientIdMessage() - unexpected size: %d != %d",
            numReceived,
            pAssignClientIdMessageOut->Header.MessageSize);
        return false;
    }

    if (pAssignClientIdMessageOut->Header.Type != MessageType::AssignClientId)
    {
        RELEASE_LOGLINE_WARNING(
            LOG_NET,
            "ReceiveAssignClientIdMessage() - unexpected message type");
        return false;
    }

    RELEASE_LOGLINE_INFO(
        LOG_NET,
        "Received 'AssignClientId' message from %s:%d",
        (pAddressOut ? pAddressOut->c_str() : "<null_address>"),
        (pPortOut ? *pPortOut : -1));
     
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
    const int sent = 
        m_spPort->send(&clientReadyMessage, ExpectedSize, pAddress, pPort);
    if (sent != ExpectedSize)
    {
        RELEASE_LOGLINE_ERROR(
            LOG_NET,
            "Failed to send ClientReady message. Sent %u, not %u",
            sent, ExpectedSize);
        return false;
    }

    RELEASE_LOGLINE_INFO(LOG_NET, "Sent ClientReady message, size = %u", sent);
    return true;
}

bool 
Protocol::ReceiveClientReadyMessage(
    ClientReadyMessage* pMessageOut,
    std::string* pAddressOut,
    int* pPortOut)
{
    RELEASE_CHECK(pMessageOut, "Invalid pMessageOut parameter");
    int numReceived = 
        m_spPort->receive(
            pMessageOut,
            sizeof(ClientReadyMessage),
            pAddressOut,
            pPortOut);
    if (numReceived <= 0)
    {
        return false;
    }

    if (numReceived != pMessageOut->Header.MessageSize)
    {
        RELEASE_LOGLINE_WARNING(
            LOG_NET,
            "ReceiveClientReadyMessage() - unexpected size: %d != %d",
            numReceived,
            pMessageOut->Header.MessageSize);
        return false;
    }

    if (pMessageOut->Header.Type != MessageType::ClientReady)
    {
        RELEASE_LOGLINE_WARNING(
            LOG_NET,
            "ReceiveClientReadyMessage() - unexpected message type");
        return false;
    }

    RELEASE_LOGLINE_INFO(
        LOG_NET,
        "Received 'ClientReady' message from %s:%d",
        (pAddressOut ? pAddressOut->c_str() : "<null_address>"),
        (pPortOut ? *pPortOut : -1));
     
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
    const int sent = 
        m_spPort->send(&startGameMessage, ExpectedSize, pAddress, pPort);
    if (sent != ExpectedSize)
    {
        RELEASE_LOGLINE_ERROR(
            LOG_NET,
            "Failed to send StartGame message. Sent %u, not %u",
            sent, ExpectedSize);
        return false;
    }

    RELEASE_LOGLINE_INFO(LOG_NET, "Sent StartGame message, size = %u", sent);
    return true;
}

bool 
Protocol::ReceiveStartGameMessage(
    StartGameMessage* pStartGameMessageOut,
    std::string* pAddressOut,
    int* pPortOut)
{
    RELEASE_CHECK(pStartGameMessageOut, "Invalid pMessageOut parameter");
    int numReceived = 
        m_spPort->receive(
            pStartGameMessageOut,
            sizeof(StartGameMessage),
            pAddressOut,
            pPortOut);
    if (numReceived <= 0)
    {
        return false;
    }

    if (numReceived != pStartGameMessageOut->Header.MessageSize)
    {
        RELEASE_LOGLINE_WARNING(
            LOG_NET,
            "ReceiveStartGameMessage() - unexpected size: %d != %d",
            numReceived,
            pStartGameMessageOut->Header.MessageSize);
        return false;
    }

    if (pStartGameMessageOut->Header.Type != MessageType::StartGame)
    {
        RELEASE_LOGLINE_WARNING(
            LOG_NET,
            "ReceiveStartGameMessage() - unexpected message type");
        return false;
    }

    RELEASE_LOGLINE_INFO(
        LOG_NET,
        "Received 'StartGame' message from %s:%d",
        (pAddressOut ? pAddressOut->c_str() : "<null_address>"),
        (pPortOut ? *pPortOut : -1));
     
    return true;
}

bool 
Protocol::ReceiveNextMessage(
    MessageHeader** ppMessageOut,
    std::string* pAddressOut,
    int* pPortOut)
{
    RELEASE_CHECK(ppMessageOut, "Invalid ppMessageOut parameter");

    MessageHeader* pMessage = m_messagePool.NewMessage();
    RELEASE_CHECK(pMessage, "Could not allocate new message");

    *ppMessageOut = nullptr;
    int numReceived = 
        m_spPort->receive(pMessage, GetMaxMessageSize(), pAddressOut, pPortOut);
    if (numReceived <= 0)
    {
        FreeMessage(pMessage);
        return false;
    }

    if (pMessage->Type == MessageType::Invalid)
    {
        RELEASE_LOGLINE_WARNING(
            LOG_NET,
            "ReceiveNextMessage() - received invalid message");
        FreeMessage(pMessage);
        return false;
    }

    if (numReceived != pMessage->MessageSize)
    {
        RELEASE_LOGLINE_WARNING(
            LOG_NET,
            "ReceiveNextMessage() - unexpected size: %d != %d",
            numReceived,
            pMessage->MessageSize);
        FreeMessage(pMessage);
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
        FreeMessage(pMessage);
        return false;
    }

    return true;
}

UDPPortPtr Protocol::GetUDPPort() const
{
    return m_spPort;
}

void Protocol::FreeMessage(void* pMessage)
{
    m_messagePool.FreeMessage(static_cast<MessageHeader*>(pMessage));
}
