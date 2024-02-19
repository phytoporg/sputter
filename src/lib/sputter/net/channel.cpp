#include "channel.h"
#include <cstring>
#include <sputter/core/check.h>
#include <sputter/log/log.h>
#include <sputter/net/port.h>

using namespace sputter;
using namespace sputter::net;

namespace 
{
    bool WriteChunkType(core::Buffer& buffer, ChunkType chunkType)
    {
        return buffer.WriteU8(static_cast<uint8_t>(chunkType));
    }

    bool ReadChunkType(core::Buffer& buffer, ChunkType* chunkType)
    {
        return buffer.ReadU8(reinterpret_cast<uint8_t*>(&chunkType));
    }
}

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
        "MESSAGE",
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
      m_sendBuffer(m_sendBufferData, sizeof(m_sendBufferData)),
      m_recvBuffer(m_recvBufferData, sizeof(m_recvBufferData))
{
    RELEASE_CHECK(spPort != nullptr, "Invalid port provided to channel");
}

void Channel::Tick()
{
    Flush();

    m_recvBuffer.Seek(0);
    const int Received = m_spPort->receive(m_recvBufferData, sizeof(m_recvBufferData));
    if (Received > 0)
    {
        DEBUG_LOGLINE_VERBOSE(LOG_NET, "Received %d bytes in channel tick", Received);
        m_recvBuffer.Seek(Received);
    }
}    

void Channel::Flush()
{
    if (m_sendBuffer.GetPosition() > 0)
    {
        // Send everything we've got!
        const int Sent = 
            m_spPort->send(m_sendBuffer.GetData(), m_sendBuffer.GetPosition());
        if (Sent != m_sendBuffer.GetPosition())
        {
            RELEASE_LOGLINE_ERROR(
                LOG_NET,
                "Sent unexpected number of bytes: %d != %u",
                Sent, m_sendBuffer.GetPosition());
        }

        // Reset the send buffer state
        m_sendBuffer.Seek(0);
    }

}

bool Channel::SendChunk(ChunkType chunkType, void* pData, size_t dataSize)
{
    RELEASE_CHECK(pData != nullptr, "pData cannot be null");

    if (!dataSize)
    {
        DEBUG_LOGLINE_WARNING(
            LOG_NET,
            "Attempted to send zero-length chunk of type %s",
            ChunkTypeToString(chunkType)
        );
        return false;
    }

    Packet* pPacket = reinterpret_cast<Packet*>(m_sendBuffer.GetData());
    if (!m_sendBuffer.GetPosition())
    {
        pPacket->ChunksMask = 0;
    }

    const uint8_t ChunkMask = ChunkTypeToBitmask(chunkType);
    if (pPacket->ChunksMask & ChunkMask)
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
    if (!WriteChunkType(m_sendBuffer, chunkType)) { return false; }
    if (!m_sendBuffer.WriteSize(dataSize)) { return false; }
    if (!m_sendBuffer.WriteBytes(pData, dataSize)) { return false; }

    pPacket->ChunksMask |= ChunkMask;
    return true;
}


bool Channel::ReceiveNextChunk(ChunkType* pChunkTypeOut, void** ppDataOut, size_t* pDataSizeOut)
{
    RELEASE_CHECK(pChunkTypeOut != nullptr, "pChunkTypeOut cannot be null");
    RELEASE_CHECK(ppDataOut != nullptr, "ppDataOut cannot be null");
    RELEASE_CHECK(pDataSizeOut != nullptr, "pDataSizeOut cannot be null");

    if (!m_recvBuffer.GetPosition())
    {
        DEBUG_LOGLINE_VERBOSE(
            LOG_NET,
            "Attempted to receive chunk, but no packet is pending");
        return false;
    }

    Packet* pPacket = static_cast<Packet*>(m_recvBuffer.GetData());
    if (!pPacket->ChunksMask)
    {
        DEBUG_LOGLINE_VERBOSE(
            LOG_NET,
            "Attempted to receive chunk, but no chunks remain in packet");
        return false;
    }

    // Find the lowest-order bit set in the chunks mask and process that chunk.
    int chunkBit = 0;
    while (!((1 << chunkBit) & pPacket->ChunksMask))
    {
        ++chunkBit;
    }

    *pChunkTypeOut = static_cast<ChunkType>(chunkBit);
    return ReceiveChunk(*pChunkTypeOut, ppDataOut, pDataSizeOut);
}

bool Channel::ReceiveChunk(ChunkType chunkType, void** ppDataOut, size_t* pDataSizeOut)
{
    if (!m_recvBuffer.GetPosition())
    {
        DEBUG_LOGLINE_VERBOSE(
            LOG_NET,
            "Attempted to receive chunk, but no packet is pending");
        return false;
    }

    core::Buffer readBuffer(
        static_cast<uint8_t*>(m_recvBuffer.GetData()), m_recvBuffer.GetPosition());

    uint8_t chunksMask;
    if (!readBuffer.ReadU8(&chunksMask))
    {
        return false;
    }

    const uint8_t ChunkTypeMask = ChunkTypeToBitmask(chunkType);
    if (!(chunksMask & ChunkTypeMask))
    {
        DEBUG_LOGLINE_VERBOSE(
            LOG_NET,
            "No pending chunk of type %s is available", ChunkTypeToString(chunkType));
        return false;
    }

    // Find the corresponding chunk data in the packet
    ChunkType nextChunkType;
    if (!ReadChunkType(readBuffer, &nextChunkType)) 
    { 
        return false; 
    }

    while (nextChunkType != chunkType)
    {
        size_t chunkSize;
        if (!readBuffer.ReadSize(&chunkSize))
        {
            return false;
        }

        if (!readBuffer.Skip(chunkSize))
        {
            return false;
        }

        if (!ReadChunkType(readBuffer, &nextChunkType)) 
        { 
            return false; 
        }
    }

    if (!readBuffer.ReadSize(pDataSizeOut))
    {
        return false;
    }
    
    *ppDataOut = readBuffer.GetDataAtPosition();

    // Remove the chunk bit corresponding to the chunk which was just read.
    Packet* pPacket = reinterpret_cast<Packet*>(readBuffer.GetData());
    pPacket->ChunksMask &= ~ChunkTypeMask;
    return true;
}

UDPPortPtr Channel::GetUDPPort() const
{
    return m_spPort;
}
