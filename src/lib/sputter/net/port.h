#pragma once

#include <string>
#include <cstdint>

namespace sputter { namespace net { 
    class UDPPort
    {
    public:
        UDPPort(uint32_t port);
        ~UDPPort();

        bool send(const void *data, int dataSize, const std::string &address, int port) const;
        int receive(void *data, int dataSize, std::string *pAddressOut = nullptr) const;

        int GetPort() const;

    private:
        int m_socketHandle = -1;
        int m_port = -1;
    };
}}