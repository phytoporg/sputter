#pragma once

#include <memory>
#include <string>

#include <sputter/net/messageprotocol.h>

namespace sputter { namespace net {
    class UDPPort;
    using UDPPortPtr = std::shared_ptr<UDPPort>;

    class Protocol
    {
    public:
        Protocol(UDPPortPtr spPort);

        bool SendHelloMessage(const std::string& name);
        bool 
        ReceiveHelloMessage(
            HelloMessage* pHelloMessageOut,
            std::string* pAddressOut = nullptr,
            int* pPortOut = nullptr);

        bool
        ReceiveNextMessage(
            MessageHeader** ppMessageOut,
            std::string* pAddressOut = nullptr,
            int* pPortOut = nullptr);

    private:
        bool ReceiveHelloMessageBody(HelloMessage* pHelloMessageOut);

        UDPPortPtr m_spPort = nullptr;
    };

    using ProtocolPtr = std::shared_ptr<Protocol>;
}}
