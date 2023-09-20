#pragma once

#include "port.h"

#include <cstdint>
#include <memory>
#include <string>

namespace sputter { namespace net {
    class UDPPort;
    class ReliableUDPSession
    {
    public:
        ReliableUDPSession(uint32_t sessionId, int localPort, const std::string& address, int remotePort);
        ReliableUDPSession(uint32_t sessionId, const UDPPort& port, const std::string& address, int remotePort);
        ~ReliableUDPSession();

        void Tick();

        size_t Send(const char* pBuffer, size_t length);
        size_t SendReliable(const char* pBuffer, size_t length);

        size_t Read(char* pBuffer, size_t maxLength);
        size_t ReadReliable(char* pBuffer, size_t maxLength);

        // Query the size of the next packet
        size_t PeekSize() const;

        void Flush();

        int GetPort() const;

    private:
        struct PImpl;
        std::unique_ptr<PImpl> m_spPimpl;
    };
}}
