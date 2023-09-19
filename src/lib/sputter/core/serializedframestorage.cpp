#include "serializedframestorage.h"
#include "check.h"

#include <cstring>

#include <sputter/memory/fixedmemoryallocator.h>
#include <sputter/log/log.h>

using namespace sputter;
using namespace sputter::core;
using namespace sputter::memory;

void SerializedFrameInfo::Reset(void* pBufferIn, uint32_t frameId)
{
    pBuffer = pBufferIn;
    FrameID = frameId;
    Size = 0;
    Checksum = 0;
}

void SerializedFrameInfo::ComputeChecksum()
{
    // Lazy checksum for now
    uint32_t sum = 0;
    const uint8_t* pByte = static_cast<const uint8_t*>(pBuffer);

    for (size_t i = 0; i < Size; ++i)
    {
        sum += pByte[i];
        DEBUG_LOGLINE_VERYVERBOSE(LOG_DEFAULT, "%llu: SUM = %u", i, sum);
    }

    Checksum = ~sum;
    DEBUG_LOGLINE_VERYVERBOSE(LOG_DEFAULT, "CHECKSUM: %llu", Checksum);
}

SerializedFrameStorage::SerializedFrameStorage(FixedMemoryAllocator &allocator, size_t frameSize)
    : m_FrameSize(frameSize)
{
    for (uint32_t i = 0; i < kMaxSerializedFrames; ++i)
    {
        m_pFramePointers[i] = allocator.ReserveNext(m_FrameSize);
        RELEASE_CHECK(m_pFramePointers[i], "Could not allocate space for serializable frame");

        m_frameInfos[i].FrameID = kInvalidFrameIndex;
    }
}

SerializedFrameInfo* SerializedFrameStorage::GetOrCreateFrame(uint32_t frameIndex)
{
    if (frameIndex > m_lastFrame || m_lastFrame == kInvalidFrameIndex)
    {
        m_lastFrame = frameIndex;
        const uint32_t FrameArrayIndex = m_lastFrame % kMaxSerializedFrames;
        RELEASE_CHECK(m_pFramePointers[FrameArrayIndex], "Frame pointer is unexpectedly null");

        SerializedFrameInfo& frameInfo = m_frameInfos[FrameArrayIndex];
        DEBUG_LOGLINE_VERBOSE(
            LOG_SERIALIZER,
            "GetOrCreateFrame: Resetting frame ID %u for %u",
            frameInfo.FrameID, frameIndex);
        frameInfo.Reset(m_pFramePointers[FrameArrayIndex], frameIndex);

        return &frameInfo;
    }
    else if (frameIndex <= m_lastFrame && 
             ((frameIndex < kMaxSerializedFrames) || 
              (frameIndex > (m_lastFrame - kMaxSerializedFrames))))
    {
        const uint32_t FrameArrayIndex = frameIndex % kMaxSerializedFrames;
        RELEASE_CHECK(m_pFramePointers[FrameArrayIndex], "Frame pointer is unexpectedly null");

        SerializedFrameInfo& frameInfo = m_frameInfos[FrameArrayIndex];
        RELEASE_CHECK(frameIndex == frameInfo.FrameID, "Retrieved wrong frame from serializer");

        DEBUG_LOGLINE_VERBOSE(
                LOG_SERIALIZER,
                "GetOrCreateFrame: Got existing frame %u",
                frameIndex);
        return &frameInfo;
    }
    else
    {
        RELEASE_CHECK(false, "Unexpected code path");
        return nullptr; // Appease the compiler
    }
}

SerializedFrameInfo* SerializedFrameStorage::CreateNextFrame()
{
    return GetOrCreateFrame(m_lastFrame + 1);
}

uint32_t SerializedFrameStorage::GetLastFrame() const
{
    return m_lastFrame;
}

uint32_t SerializedFrameStorage::GetFrameSize() const
{
    return m_FrameSize;
}

void SerializedFrameStorage::Reset()
{
    m_lastFrame = kInvalidFrameIndex;
    memset(m_frameInfos, 0, sizeof(m_frameInfos));
}