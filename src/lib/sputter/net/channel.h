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
        MESSAGE          = 0,
        INPUTS           = 1,
        CLIENT_STATUS    = 2,
        SERVER_STATUS    = 3,
        MAX_VALUE        = 4,
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

        // Call ahead of Tick/Flush, queue chunks for the next packet across the wire
        bool SendChunk(ChunkType chunkType, void* pData, size_t dataSize);

        // Call *after* Tick to read chunks that have been received.
        bool ReceiveNextChunk(
            ChunkType* pChunkTypeOut, void** ppDataOut, size_t* pDataSizeOut);
        bool ReceiveChunk(ChunkType chunkType, void** ppDataOut, size_t* pDataSizeOut);

        UDPPortPtr GetUDPPort() const;

    private:
        struct Packet
        {
            uint8_t ChunksMask = 0;
            uint8_t Data[];
        };

        Packet* m_pCurrentSendPacket = nullptr;
        uint8_t m_sendBufferData[net::kMTU];
        core::Buffer m_sendBuffer;

        Packet* m_pCurrentRecvPacket = nullptr;
        uint8_t m_recvBufferData[net::kMTU];
        core::Buffer m_recvBuffer;

        UDPPortPtr  m_spPort = nullptr;
    };
}}

