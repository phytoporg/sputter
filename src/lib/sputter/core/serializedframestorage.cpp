#include "serializedframestorage.h"
#include "check.h"

#include <sputter/memory/fixedmemoryallocator.h>

using namespace sputter;
using namespace sputter::core;
using namespace sputter::memory;

SerializedFrameStorage::SerializedFrameStorage(FixedMemoryAllocator &allocator, size_t frameSize)
    : m_FrameSize(frameSize)
{
    for (uint32_t i = 0; i < kMaxSerializedFrames; ++i)
    {
        m_pFramePointers[i] = allocator.ReserveNext(m_FrameSize);
        RELEASE_CHECK(m_pFramePointers[i], "Could not allocate space for serializable frame");

        m_frameIds[i] = kInvalidFrameIndex;
    }
}

void* SerializedFrameStorage::GetOrCreateFrame(uint32_t frameIndex)
{
    if (frameIndex == (m_lastFrame + 1))
    {
        ++m_lastFrame;
        const uint32_t FrameArrayIndex = m_lastFrame % kMaxSerializedFrames;
        RELEASE_CHECK(m_pFramePointers[FrameArrayIndex], "Frame pointer is unexpectedly null");
        return m_pFramePointers[FrameArrayIndex];
    }
    else if (frameIndex <= m_lastFrame && 
             ((frameIndex < kMaxSerializedFrames) || 
              (frameIndex > (m_lastFrame - kMaxSerializedFrames))))
    {
        const uint32_t FrameArrayIndex = frameIndex % kMaxSerializedFrames;
        RELEASE_CHECK(m_pFramePointers[FrameArrayIndex], "Frame pointer is unexpectedly null");
        return m_pFramePointers[FrameArrayIndex];
    }
    else
    {
        return nullptr; // Appease the compiler
    }
}

uint32_t SerializedFrameStorage::GetLastFrame() const
{
    return m_lastFrame;
}