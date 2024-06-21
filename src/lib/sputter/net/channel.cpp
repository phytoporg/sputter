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

Channel::Channel(UDPPortPtr spPort, const std::string& channelName) 
    : m_spPort(spPort),
      m_sendBuffer(m_sendBufferData, sizeof(m_sendBufferData)),
      m_recvBuffer(m_recvBufferData, sizeof(m_recvBufferData)),
      m_channelName(channelName)
{
    RELEASE_CHECK(spPort != nullptr, "Invalid port provided to channel");
}

void Channel::Tick()
{
    Flush();

    if (!ReceivePacket(&m_lastReceivedAddress, &m_lastReceivedPort))
    {
        RELEASE_LOGLINE_ERROR(
            LOG_NET,
            "Failed to receive packet in channel %s",
            m_channelName.c_str());
    }
}    

void Channel::Flush()
{
    if (m_sendBuffer.GetPosition() > 0)
    {
        Packet* pPacket = reinterpret_cast<Packet*>(m_sendBuffer.GetData());
        pPacket->DataSize = m_sendBuffer.GetPosition() - Packet::HeaderSize();

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
        else
        {
            DEBUG_LOGLINE_VERBOSE(
                LOG_NET, "Sent %d bytes in channel flush, packet size = %d, pos = %d",
                Sent, 
                pPacket->DataSize,
                m_sendBuffer.GetPosition());
        }

        // Reset the send buffer state
        m_sendBuffer.Seek(0);
    }

}

bool Channel::ReceivePacket(std::string* pAddressOut, int* pPortOut)
{
    // Reset the receive buffer
    m_recvBuffer.Seek(0);

    // Wait until we receive all expected bits (TODO: handle timeout)
    uint8_t buffer[net::kMTU];
    int totalReceived = 0;
    const int Received =
        m_spPort->receive(buffer, sizeof(buffer), pAddressOut, pPortOut);
    if (Received > 0)
    {
        DEBUG_LOGLINE_VERBOSE(
            LOG_NET, "Received %d bytes in channel ReceivePacket", Received);
        if (!m_recvBuffer.WriteBytes(buffer, Received))
        {
            RELEASE_LOGLINE_ERROR(
                LOG_NET,
                "Could not store all received bytes (Received = %u)",
                Received);
            return false;
        }

        Packet* pPacket = reinterpret_cast<Packet*>(m_recvBuffer.GetData());
        if (!pPacket->ChunksMask)
        {
            RELEASE_LOGLINE_ERROR(
                LOG_NET,
                "Received packet with no chunks");
            return false;
        }

        const int ExpectedDataSize = Received - Packet::HeaderSize();
        if (pPacket->DataSize != ExpectedDataSize)
        {
            RELEASE_LOGLINE_ERROR(
                LOG_NET,
                "Received packet with unexpected size (%u != %u)",
                pPacket->DataSize,
                ExpectedDataSize);
            return false;
        }
    }
    else if (Received < 0)
    {
        return false;
    }

    return true;
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
        // Initialize and make room for the packet header
        pPacket->ChunksMask = 0;
        pPacket->DataSize = 0;
        m_sendBuffer.Skip(Packet::HeaderSize());
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

    DEBUG_LOGLINE_VERBOSE(
        LOG_NET,
        "Sending %s chunk",
        ChunkTypeToString(chunkType)
    );

    return true;
}


bool Channel::ReceiveNextChunk(ChunkType* pChunkTypeOut, void** ppDataOut, size_t* pDataSizeOut)
{
    RELEASE_CHECK(pChunkTypeOut != nullptr, "pChunkTypeOut cannot be null");
    RELEASE_CHECK(ppDataOut != nullptr, "ppDataOut cannot be null");
    RELEASE_CHECK(pDataSizeOut != nullptr, "pDataSizeOut cannot be null");

    if (!m_recvBuffer.GetPosition())
    {
        // DEBUG_LOGLINE_VERBOSE(
        //     LOG_NET,
        //     "Attempted to receive chunk, but no packet is pending");
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
        // DEBUG_LOGLINE_VERBOSE(
        //     LOG_NET,
        //     "Attempted to receive chunk, but no packet is pending");
        return false;
    }

    Packet* pPacket = reinterpret_cast<Packet*>(m_recvBuffer.GetData());

    core::Buffer readBuffer(
        static_cast<uint8_t*>(m_recvBuffer.GetData()), pPacket->GetSize());
    readBuffer.Skip(Packet::HeaderSize());

    uint8_t chunksMask = pPacket->ChunksMask;

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
        DEBUG_LOGLINE_VERBOSE(
            LOG_NET,
            "Could not read next chunk type in packet");
        return false; 
    }

    // Skip each chunk until we find the one we're looking for
    while (nextChunkType != chunkType)
    {
        size_t chunkSize;
        if (!readBuffer.ReadSize(&chunkSize))
        {
            DEBUG_LOGLINE_VERBOSE(
                LOG_NET,
                "Failed to read chunk size in packet");
            return false;
        }

        if (!readBuffer.Skip(chunkSize))
        {
            DEBUG_LOGLINE_VERBOSE(
                LOG_NET,
                "Ran out of buffer space skipping %u bytes", chunkSize);
            return false;
        }

        if (!ReadChunkType(readBuffer, &nextChunkType)) 
        { 
            DEBUG_LOGLINE_VERBOSE(
                LOG_NET,
                "Failed to read next chunk type in packet");
            return false; 
        }
    }

    size_t chunkSize;
    if (!readBuffer.ReadSize(&chunkSize))
    {
        DEBUG_LOGLINE_VERBOSE(
            LOG_NET,
            "Could not read last size value in packet (huh?)");
        return false;
    }

    DEBUG_LOGLINE_VERBOSE(
        LOG_NET,
        "Found chunk [type: %s, size: %u]",
        ChunkTypeToString(nextChunkType),
        chunkSize);

    // Sanity check the chunk size
    if (chunkSize >= pPacket->GetSize())
    {
        RELEASE_LOGLINE_INFO(
            LOG_NET,
            "Chunk size is too large: %u",
            chunkSize);
        return false;
    }

    *pDataSizeOut = chunkSize;
    *ppDataOut = readBuffer.GetDataAtPosition();

    // Remove the chunk bit corresponding to the chunk which was just read.
    pPacket->ChunksMask &= ~ChunkTypeMask;
    return true;
}

UDPPortPtr Channel::GetUDPPort() const
{
    return m_spPort;
}

const std::string& Channel::GetLastReceivedAddress() const
{
    return m_lastReceivedAddress;
}

int Channel::GetLastReceivedPort() const
{
    return m_lastReceivedPort;
}
