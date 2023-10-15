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

        //
        // Hello
        //
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

        //
        // AssignClientId
        //
        bool
        SendAssignClientIdMessage(
            uint8_t clientId,
            const std::string* pAddress = nullptr,
            const int* pPort = nullptr);
        bool 
        ReceiveAssignClientIdMessage(
            AssignClientIdMessage* pAssignClientIdMessageOut,
            std::string* pAddressOut = nullptr,
            int* pPortOut = nullptr);

        //
        // ClientReady
        //
        bool
        SendClientReadyMessage(
            uint8_t clientId,
            const std::string* pAddress = nullptr,
            const int* pPort = nullptr);
        bool 
        ReceiveClientReadyMessage(
            ClientReadyMessage* pClientReadyMessageOut,
            std::string* pAddressOut = nullptr,
            int* pPortOut = nullptr);

        //
        // StartGame
        //
        bool
        SendStartGameMessage(
            uint32_t gameID,
            const std::string* pAddress = nullptr,
            const int* pPort = nullptr);
        bool 
        ReceiveStartGameMessage(
            StartGameMessage* pStartGameMessage,
            std::string* pAddressOut = nullptr,
            int* pPortOut = nullptr);

        //
        // General
        //
        bool
        ReceiveNextMessage(
            MessageHeader** ppMessageOut,
            std::string* pAddressOut = nullptr,
            int* pPortOut = nullptr);

        UDPPortPtr GetUDPPort() const;

        void FreeMessage(void* pMessage);

    private:
        UDPPortPtr  m_spPort = nullptr;
        MessagePool m_messagePool;
    };

    using ProtocolPtr = std::shared_ptr<Protocol>;
}}
