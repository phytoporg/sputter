#include "channel.h"
#include <cstring>
#include <sputter/core/check.h>
#include <sputter/log/log.h>

using namespace sputter;
using namespace sputter::net;

uint8_t sputter::net::ChunkTypeToBitmask(ChunkType chunkType)
{
    if (chunkType > ChunkType::MAX_VALUE)
    {
        return 0;
    }

    const uint8_t ChunkTypeAsByte = static_cast<uint8_t>(chunkType);
    return 1 << ChunkTypeAsByte;
}

const char* sputter::net::ChunkTypeToString(ChunkType chunkType)
{
    RELEASE_CHECK(chunkType < ChunkType::MAX_VALUE, "Invalid chunk type value");

    const char* pLUT[] = {
        "HELLO",
        "ASSIGN_CLIENT_ID",
        "CLIENT_READY",
        "GAME_START",
        "INPUTS",
        "CLIENT_STATUS",
        "SERVER_STATUS",
    };

    const uint8_t ChunkTypeByte = static_cast<uint8_t>(chunkType);
    return pLUT[ChunkTypeByte];
}

void BitmaskToChunkTypes(
    uint8_t bitmask,
    ChunkType chunkTypes[],
    int* pNumChunkTypesOut)
{
    RELEASE_CHECK(pNumChunkTypesOut, "pNumChunkTypesOut cannot be null");

    *pNumChunkTypesOut = 0;
    const uint8_t MaxBit = static_cast<uint8_t>(ChunkType::MAX_VALUE);
    for (uint8_t bitIndex = 0; bitIndex < MaxBit; ++bitIndex)
    {
        if (bitmask & (1 << bitIndex))
        {
            chunkTypes[*pNumChunkTypesOut] = static_cast<ChunkType>(bitIndex);
            (*pNumChunkTypesOut)++;
        }
    }
}

Channel::Channel(UDPPortPtr spPort) 
    : m_spPort(spPort),
      m_sendBuffer(m_sendBufferData, sizeof(m_sendBufferData))
{
    RELEASE_CHECK(spPort != nullptr, "Invalid port provided to channel");
}

void Tick()
{
    // TODO
}    

void Flush()
{
    // TODO
}

bool Channel::SendChunk(ChunkType chunkType, void* pData, size_t dataSize)
{
    if (!dataSize)
    {
        DEBUG_LOGLINE_WARNING(
            LOG_NET,
            "Attempted to send zero-length chunk of type %s",
            ChunkTypeToString(chunkType)
        );
        return false;
    }

    if (!m_pCurrentSendPacket)
    {
        m_pCurrentSendPacket = reinterpret_cast<Packet*>(m_sendBuffer.GetData());
        m_pCurrentSendPacket->ChunksMask = 0;
    }

    const uint8_t ChunkMask = ChunkTypeToBitmask(chunkType);
    if (m_pCurrentSendPacket->ChunksMask & ChunkMask)
    {
        DEBUG_LOGLINE_WARNING(
            LOG_NET,
            "Attempting to send multiple chunks of type %s",
            ChunkTypeToString(chunkType)
        );
        return false;
    }

    // Is there enough room left to append the specified chunk?
    const size_t BytesRemaining = m_sendBuffer.BytesRemaining();
    if (BytesRemaining < dataSize)
    {
        DEBUG_LOGLINE_ERROR(
            LOG_NET,
            "Insufficient bytes remaining to send chunk of type %s",
            ChunkTypeToString(chunkType)
        );
        return false;
    }

    // We have room, write the data
    m_sendBuffer.WriteU8(static_cast<uint8_t>(chunkType));
    m_sendBuffer.WriteSize(dataSize);
    m_sendBuffer.WriteBytes(pData, dataSize);

    return true;
}

bool Channel::ReceiveChunk()
{
    // TODO
    return false;
}
