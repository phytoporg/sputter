#pragma once

// TODO: This could be a general object pool?

#include <sputter/net/messageprotocol.h>

#include <cstdint>
#include <cstddef>

namespace sputter { namespace net {
    class MessagePool
    {
    public:
        MessagePool(size_t numPoolEntries);

        MessageHeader* NewMessage();
        void FreeMessage(MessageHeader* pMesageHeader);

    private:
        static constexpr size_t kMaxMessagePoolSize = 124;
        static constexpr size_t kMaxMessageSize = 1500; // MTU?

        using MessagePoolEntry = uint8_t[kMaxMessageSize];
        MessagePoolEntry m_messagePoolEntries[kMaxMessagePoolSize];
        bool m_poolOccupancy[kMaxMessagePoolSize] = {};

        size_t m_numPoolEntries = 0;
        size_t m_currentEntryCount = 0;
    };
}}
