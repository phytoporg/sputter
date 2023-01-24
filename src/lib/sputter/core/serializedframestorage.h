#pragma once

#include <cstdint>
#include <cstddef>

// Forward declarations
namespace sputter { namespace memory {
    class FixedMemoryAllocator;
}}

namespace sputter { namespace core {
    struct SerializedFrameInfo
    {
        void*    pBuffer = nullptr;
        uint32_t FrameID = 0;
        size_t   Size = 0;
        uint32_t Checksum = 0;

        void Reset(void* pBufferIn, uint32_t frameId);
        void ComputeChecksum();
    };

    class SerializedFrameStorage
    {
    public:
        SerializedFrameStorage(memory::FixedMemoryAllocator& allocator, size_t frameSize);

        SerializedFrameInfo* GetOrCreateFrame(uint32_t frameIndex);
        SerializedFrameInfo* CreateNextFrame();
        uint32_t GetLastFrame() const;
        uint32_t GetFrameSize() const;

    private:
        SerializedFrameStorage() = delete;
        SerializedFrameStorage(const SerializedFrameStorage& other) = delete;

        static const uint32_t kMaxSerializedFrames         = 8;
        void*    m_pFramePointers[kMaxSerializedFrames]    = {};
        SerializedFrameInfo m_frameInfos[kMaxSerializedFrames] = {};

        static const uint32_t kInvalidFrameIndex = static_cast<uint32_t>(-1);;
        uint32_t m_lastFrame = kInvalidFrameIndex;

        const uint32_t m_FrameSize;
    };
}}