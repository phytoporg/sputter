#pragma once

#include <functional>
#include <cstdint>
#include <vector>
#include <memory>
#include <string>

#include <sputter/net/port.h>
#include <sputter/net/protocol.h>
#include <sputter/net/messageprotocol.h>

class Server
{
public:
    using ClientHandle = int32_t;
    static constexpr ClientHandle kInvalidHandle = -1;
    using ClientConnectionCallback = std::function<void(Server*, ClientHandle)>;

    static constexpr int kDefaultServerPort = 7001;
    static constexpr int kDefaultClientPort = 7000;
    Server(ClientConnectionCallback connectionCallbackFn, int port = kDefaultServerPort);

    bool Listen();
    void Tick();

    void SetShouldTerminate();
    bool ShouldTerminate() const;
    int GetPort() const;

    bool GetClientAddress(ClientHandle handle, std::string& addressOut) const;
    bool GetClientPort(ClientHandle handle, int& portOut) const;

private:
    Server() = delete;

    // Private helpers
    void ReceiveMessages();
    bool HandleReceiveHello(
        HelloMessage* pHelloMessage,
        const std::string& address,
        int port);

    bool FindClient(const std::string& name, const std::string& address, int port);

    int m_listenPort = kDefaultServerPort;
    ClientConnectionCallback m_connectionCallback;
    bool m_shouldTerminate = false;

    sputter::net::UDPPortPtr  m_spListenPort = nullptr;
    sputter::net::ProtocolPtr m_spProtocol = nullptr;

    struct ClientConnection
    {
        std::string Name;
        std::string Address;
        int Port = -1;
    };
    std::vector<ClientConnection> m_clientConnections;
};
