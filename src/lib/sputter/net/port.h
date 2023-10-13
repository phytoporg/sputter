#pragma once

#include <string>
#include <cstdint>
#include <memory>

namespace sputter { namespace net { 
    class UDPPort
    {
    public:
        UDPPort(int port);
        UDPPort(const UDPPort& port);
        ~UDPPort();

        bool connect(const std::string& address, int remotePort);
        bool bind();
        int 
        send(
            const void *data,
            int dataSize,
            const std::string& address = "",
            int port = -1) const;
        int 
        receive(
            void *data,
            int dataSize,
            std::string *pAddressOut = nullptr,
            int* pPortOut = nullptr) const;

        int GetSocket() const;
        int GetPort() const;

        std::string GetRemoteAddress() const;
        int GetRemotePort() const;

        bool IsBound() const;

    private:
        int m_socketHandle = -1;
        int m_port = -1;

        std::string m_remoteAddress;
        int         m_remotePort = -1;
    };

    using UDPPortPtr = std::shared_ptr<UDPPort>;
}}
