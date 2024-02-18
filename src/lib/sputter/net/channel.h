#pragma once

#include <cstdint>
#include <memory>

#include <sputter/core/buffer.h>
#include <sputter/net/net.h>

namespace sputter { namespace net {
    class UDPPort;
    using UDPPortPtr = std::shared_ptr<UDPPort>;

    enum class ChunkType : uint8_t
    {
        HELLO            = 0,
        ASSIGN_CLIENT_ID = 1,
        CLIENT_READY     = 2,
        GAME_START       = 3,
        INPUTS           = 4,
        CLIENT_STATUS    = 5,
        SERVER_STATUS    = 6,
        MAX_VALUE        = 7,
    };
    static uint8_t ChunkTypeToBitmask(ChunkType chunkType);
    static const char* ChunkTypeToString(ChunkType chunkType);
    static void BitmaskToChunkTypes(
        uint8_t bitmask,
        ChunkType chunkTypes[],
        int* pNumChunkTypesOut);

    class Channel
    {
    public:
        Channel(UDPPortPtr spPort);

        void Tick();
        void Flush();

        bool SendChunk(ChunkType chunkType, void* pData, size_t dataSize);
        bool ReceiveChunk();

    private:
        struct Packet
        {
            uint8_t ChunksMask = 0;
            uint8_t Data[];
        };

        Packet* m_pCurrentSendPacket = nullptr;
        uint8_t m_sendBufferData[net::kMTU];
        core::Buffer m_sendBuffer;

        UDPPortPtr  m_spPort = nullptr;
    };
}}

