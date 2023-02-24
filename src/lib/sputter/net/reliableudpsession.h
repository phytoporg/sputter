#pragma once

#include <cstdint>
#include <memory>
#include <string>

// Forward declaration
struct IKCPCB;

namespace sputter { namespace net {
    class ReliableUDPSession
    {
    public:
        ReliableUDPSession(uint32_t sessionId, const std::string& address, int port);
        ~ReliableUDPSession();

        void Tick();

        // Queues data to be sent next tick
        size_t EnqueueSendData(const char* pBuffer, size_t length);

        // Returns the number of bytes actually read
        size_t TryReadData(char* pBuffer, size_t maxLength);

    private:
        static int SendDataCallback(const char* pBuffer, int length, struct IKCPCB* pKcp, void* pUser);

        struct PImpl;
        std::unique_ptr<PImpl> m_spPimpl;
    };
}}