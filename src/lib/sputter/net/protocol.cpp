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

bool Protocol::SendHelloMessage(const std::string& name)
{
    HelloMessage helloMessage;
    if (!CreateHelloMessage(name.c_str(), name.size(), helloMessage))
    {
        RELEASE_LOGLINE_ERROR(LOG_NET, "Failed to create hello message");
        return false;
    }

    const size_t ExpectedSize = 
        HelloMessage::GetExpectedSize(name.c_str(), name.size());
    const int sent = m_spPort->send(&helloMessage, ExpectedSize);
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
            &pHelloMessageOut,
            sizeof(MessageHeader),
            pAddressOut,
            pPortOut);
    if (numReceived != sizeof(MessageHeader))
    {
        return false;
    }
     
    return ReceiveHelloMessageBody(pHelloMessageOut);
}

bool 
Protocol::ReceiveNextMessage(
    MessageHeader** ppMessageOut,
    std::string* pAddressOut,
    int* pPortOut)
{
    RELEASE_CHECK(ppMessageOut, "Invalid ppMessageOut parameter");

    *ppMessageOut = nullptr;
    MessageHeader header;
    int numReceived = 
        m_spPort->receive(&header, sizeof(header), pAddressOut, pPortOut);
    if (numReceived <= 0)
    {
        return false;
    }

    if (numReceived != sizeof(header) || header.Type == MessageType::Invalid)
    {
        RELEASE_LOGLINE_WARNING(
            LOG_NET,
            "ReceiveNextMessage() - unexpected size: %d != %d",
            numReceived,
            sizeof(header));
        return false;
    }

    if (header.Type == MessageType::Hello)
    {
        RELEASE_LOGLINE_INFO(
            LOG_NET,
            "Received 'Hello' message from %s:%d",
            (pAddressOut ? pAddressOut->c_str() : "NA"),
            (pPortOut ? *pPortOut : -1));

        // TODO: use a pool?
        HelloMessage* pMessage = new HelloMessage;
        memcpy(&pMessage->Header, &header, sizeof(header));
        if (!ReceiveHelloMessageBody(pMessage))
        {
            RELEASE_LOGLINE_WARNING(LOG_NET, "Failed to parse hello message body");
            delete pMessage;
            return false;
        }

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

bool Protocol::ReceiveHelloMessageBody(HelloMessage* pHelloMessageOut)
{
    if (pHelloMessageOut->Header.Type != MessageType::Hello)
    {
        RELEASE_LOGLINE_ERROR(LOG_NET, "Unexpected header type");
        return false;
    }

    int numReceived =
        m_spPort->receive(
            &pHelloMessageOut->NameSize, sizeof(pHelloMessageOut->NameSize));
    if (numReceived <= 0)
    {
        return false;
    }

    if (pHelloMessageOut->NameSize == 0)
    {
        RELEASE_LOGLINE_ERROR(LOG_NET, "Hello message has no name");
        return false;
    }

    memset(pHelloMessageOut->Name, 0, sizeof(pHelloMessageOut->Name));
    numReceived = m_spPort->receive(pHelloMessageOut->Name, pHelloMessageOut->NameSize);
    if (numReceived != pHelloMessageOut->NameSize)
    {
        RELEASE_LOGLINE_ERROR(LOG_NET, "Failed to read name from Hello message");
        return false;
    }

    return true;
}

