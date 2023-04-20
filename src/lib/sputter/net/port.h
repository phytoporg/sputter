#pragma once

#include <string>
#include <cstdint>

namespace sputter { namespace net { 
    class UDPPort
    {
    public:
        UDPPort(uint32_t port);

        // TODO: right now the above constructor auto-binds. We want this to be a much
        // dumber implementation, but for now we'll leave it so we can keep tests passing
        // and iteratively migrate. In the meantime, this is the alternate set of port
        // construction/initialization functions.
        UDPPort();
        void SetSocket(int newSocket);
        void SetPort(int newPort);
        void Close();

        ~UDPPort();

        int send(const void *data, int dataSize, const std::string &address, int port) const;
        int receive(void *data, int dataSize, std::string *pAddressOut = nullptr) const;

        int GetSocket() const;
        int GetPort() const;

        bool IsBound() const;

    private:
        int m_socketHandle = -1;
        int m_port = -1;
    };
}}