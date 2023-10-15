#pragma once

#include <memory>
#include <string>

#include <sputter/net/messageprotocol.h>
#include <sputter/net/messagepool.h>

namespace sputter { namespace net {
    class UDPPort;
    using UDPPortPtr = std::shared_ptr<UDPPort>;

    class Protocol
    {
    public:
        Protocol(UDPPortPtr spPort);

        bool 
        SendHelloMessage(
            const std::string& name,
            const std::string* pAddress = nullptr,
            const int* pPort = nullptr);
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

        void FreeMessage(void* pMessage);

    private:
        UDPPortPtr  m_spPort = nullptr;
        MessagePool m_messagePool;
    };

    using ProtocolPtr = std::shared_ptr<Protocol>;
}}
