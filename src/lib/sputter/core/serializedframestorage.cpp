#include "serializedframestorage.h"
#include "check.h"

#include <sputter/memory/fixedmemoryallocator.h>

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
    }

    Checksum = ~sum;
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
        frameInfo.Reset(m_pFramePointers[FrameArrayIndex], frameIndex);

        return &frameInfo;
    }
    else
    {
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