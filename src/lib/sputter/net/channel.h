#pragma once

#include <cstdint>
#include <memory>
#include <string>

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
        Channel(UDPPortPtr spPort, const std::string& channelName);

        void Tick();
        void Flush();

        // Call ahead of Tick/Flush, queue chunks for the next packet across the wire
        bool SendChunk(ChunkType chunkType, void* pData, size_t dataSize);

        // Call *after* Tick to read chunks that have been received.
        bool ReceiveNextChunk(
            ChunkType* pChunkTypeOut, void** ppDataOut, size_t* pDataSizeOut);
        bool ReceiveChunk(ChunkType chunkType, void** ppDataOut, size_t* pDataSizeOut);

        UDPPortPtr GetUDPPort() const;

        const std::string& GetLastReceivedAddress() const;
        int GetLastReceivedPort() const;

    private:
        bool ReceivePacket(
            std::string* pAddressOut = nullptr, int* pPortOut = nullptr);

        struct Packet
        {
            static const size_t HeaderSize() 
            { 
                return sizeof(ChunksMask) + sizeof(DataSize);
            }

            uint16_t GetSize()
            {
                return HeaderSize() + DataSize;
            }

            uint8_t ChunksMask = 0;
            uint16_t DataSize = 0;
            uint8_t Data[];
        };

        uint8_t m_sendBufferData[net::kMTU];
        core::Buffer m_sendBuffer;

        uint8_t m_recvBufferData[net::kMTU];
        core::Buffer m_recvBuffer;

        std::string m_lastReceivedAddress;
        int         m_lastReceivedPort = -1;

        UDPPortPtr  m_spPort = nullptr;

        std::string m_channelName = "unnamed";
    };
}}

