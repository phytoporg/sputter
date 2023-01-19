#pragma once

#include <cstdint>
#include <cstddef>

// Forward declarations
namespace sputter { namespace memory {
    class FixedMemoryAllocator;
}}

namespace sputter { namespace core {
    class SerializedFrameStorage
    {
    public:
        SerializedFrameStorage(memory::FixedMemoryAllocator& allocator, size_t frameSize);

        void* GetOrCreateFrame(uint32_t frameIndex);
        void* CreateNextFrame();
        uint32_t GetLastFrame() const;
        uint32_t GetFrameSize() const;

    private:
        SerializedFrameStorage() = delete;
        SerializedFrameStorage(const SerializedFrameStorage& other) = delete;

        static const uint32_t kMaxSerializedFrames      = 8;
        void*    m_pFramePointers[kMaxSerializedFrames] = {};
        uint32_t m_frameIds[kMaxSerializedFrames]       = {};

        static const uint32_t kInvalidFrameIndex = static_cast<uint32_t>(-1);;
        uint32_t m_lastFrame = kInvalidFrameIndex;

        const uint32_t m_FrameSize;
    };
}}