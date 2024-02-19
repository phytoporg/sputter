#pragma once

#include <memory>
#include <string>
#include <functional>

#include <sputter/net/channel.h>
#include <sputter/net/messageprotocol.h>

namespace sputter { namespace net {
    class UDPPort;
    using UDPPortPtr = std::shared_ptr<UDPPort>;

    class Protocol
    {
    public:
        using MessageReceivedCallback =
            std::function<void(MessageHeader*, const std::string&, int)>;

        Protocol(UDPPortPtr spPort);

        void Tick();
        void SetMessageReceivedCallback(MessageReceivedCallback callback);

        //
        // Hello
        //
        bool 
        SendHelloMessage(
            const std::string& name,
            const std::string* pAddress = nullptr,
            const int* pPort = nullptr);

        //
        // AssignClientId
        //
        bool
        SendAssignClientIdMessage(
            uint8_t clientId,
            const std::string* pAddress = nullptr,
            const int* pPort = nullptr);

        //
        // ClientReady
        //
        bool
        SendClientReadyMessage(
            uint8_t clientId,
            const std::string* pAddress = nullptr,
            const int* pPort = nullptr);

        //
        // StartGame
        //
        bool
        SendStartGameMessage(
            uint32_t gameID,
            const std::string* pAddress = nullptr,
            const int* pPort = nullptr);

        //
        // General
        //
        bool
        ReceiveNextMessage(
            MessageHeader** ppMessageOut,
            std::string* pAddressOut = nullptr,
            int* pPortOut = nullptr);

        UDPPortPtr GetUDPPort() const;

    private:
        Channel                 m_channel;
        MessageReceivedCallback m_messageCallback;
    };

    using ProtocolPtr = std::shared_ptr<Protocol>;
}}
