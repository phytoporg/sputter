#pragma once

#include <functional>
#include <cstdint>
#include <vector>
#include <memory>
#include <string>

#include <sputter/net/port.h>

class Server
{
public:
    using ClientHandle = int32_t;
    static constexpr ClientHandle kInvalidHandle = -1;
    using ClientConnectionCallback = std::function<void(ClientHandle)>;

    static constexpr int kDefaultServerPort = 5000;
    static constexpr int kDefaultClientPort = 5001;
    Server(ClientConnectionCallback connectionCallbackFn, int port = kDefaultServerPort);

    bool Listen();
    void Tick();

    void SetShouldTerminate();
    bool ShouldTerminate() const;
    int GetPort() const;

private:
    Server() = delete;

    int m_listenPort = kDefaultServerPort;
    ClientConnectionCallback m_connectionCallback;
    bool m_shouldTerminate = false;

    using PortPtr = std::shared_ptr<sputter::net::UDPPort>;
    PortPtr m_spListenPort = nullptr;

    struct ClientConnection
    {
        std::string Address;
    };
    std::vector<ClientConnection> m_clientConnections;
};
