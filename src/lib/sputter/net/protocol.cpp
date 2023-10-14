#include "protocol.h"
#include "port.h"
#include "messageprotocol.h"

#include <cstring>
#include <sputter/core/check.h>
#include <sputter/log/log.h>

using namespace sputter;
using namespace sputter::net;

Protocol::Protocol(UDPPortPtr spPort)
    : m_spPort(spPort) 
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

    RELEASE_LOGLINE_INFO(
        LOG_NET,
        "Received 'Hello' message from %s:%d",
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

    // TODO: use a message pool!
    char buffer[GetMaxMessageSize()];

    *ppMessageOut = nullptr;
    int numReceived = 
        m_spPort->receive(buffer, sizeof(buffer), pAddressOut, pPortOut);
    if (numReceived <= 0)
    {
        return false;
    }

    auto pHeader = reinterpret_cast<MessageHeader*>(buffer);
    if (pHeader->Type == MessageType::Invalid)
    {
        RELEASE_LOGLINE_WARNING(
            LOG_NET,
            "ReceiveNextMessage() - received invalid message");
        return false;
    }

    if (numReceived != pHeader->MessageSize)
    {
        RELEASE_LOGLINE_WARNING(
            LOG_NET,
            "ReceiveNextMessage() - unexpected size: %d != %d",
            numReceived,
            pHeader->MessageSize);
        return false;
    }

    if (pHeader->Type == MessageType::Hello)
    {
        RELEASE_LOGLINE_INFO(
            LOG_NET,
            "Received 'Hello' message from %s:%d",
            (pAddressOut ? pAddressOut->c_str() : "<null_address>"),
            (pPortOut ? *pPortOut : -1));

        // TODO: (again) use a message pool
        HelloMessage* pMessage = new HelloMessage;
        memcpy(pMessage, buffer, pHeader->MessageSize);
        *ppMessageOut = reinterpret_cast<MessageHeader*>(pMessage);
    }
    else
    {
        // TODO: Support other message types
        RELEASE_LOGLINE_WARNING(LOG_NET, "Received unexpected message type");
        return false;
    }

    return true;
}

