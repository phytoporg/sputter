#pragma once

#include "port.h"

#include <cstdint>
#include <memory>
#include <string>

// Forward declaration
struct IKCPCB;

namespace sputter { namespace net {
    class ReliableUDPSession
    {
    public:
        ReliableUDPSession(uint32_t sessionId, int localPort, const std::string& address, int remotePort);
        ReliableUDPSession(uint32_t sessionId, const UDPPort& port, const std::string& address, int remotePort);
        ~ReliableUDPSession();

        void Tick();

        // Queues data to be sent next tick
        size_t EnqueueSendData(const char* pBuffer, size_t length);

        // Returns the number of bytes actually read
        size_t TryReadData(char* pBuffer, size_t maxLength);

        // Query the size of the next packet
        size_t PeekSize() const;

        void Flush();

        int GetPort() const;

    private:
        static int SendDataCallback(const char* pBuffer, int length, struct IKCPCB* pKcp, void* pUser);

        struct PImpl;
        std::unique_ptr<PImpl> m_spPimpl;
    };
}}