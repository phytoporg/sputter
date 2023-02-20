#pragma once

#include <cstdint>
#include <memory>

namespace sputter { namespace net {
    class ReliableUDPSession
    {
    public:
        ReliableUDPSession(uint32_t sessionId);
        ~ReliableUDPSession();

        void Tick();

        // Queues data to be sent next tick
        void EnqueueSendData(const char* pBuffer, size_t length);

        // Returns the number of bytes actually read
        size_t TryReadData(char* pBuffer, size_t maxLength);

    private:
        static void SendDataCallback(const char* pBuffer, int length);

        struct PImpl;
        std::shared_ptr<PImpl> m_spPimpl;
    };
}}