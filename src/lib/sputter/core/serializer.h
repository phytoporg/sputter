#pragma once

// The serializer tracks all objects needing serialization, and 
// serializes or deserializes their state into or from a contiguous
// buffer, respectively.

#include <cstdint>
#include <cstddef>

#include "serializedframestorage.h"

// Forward declarations
namespace sputter { namespace memory {
    class FixedMemoryAllocator;
}}

namespace sputter { namespace core {
    class ISerializable;

    class Serializer
    {
    public:
        Serializer(memory::FixedMemoryAllocator& allocator);

        bool RegisterSerializableObject(ISerializable* pSerializableObject);
        bool UnregisterSerializableObject(ISerializable* pSerializableObject);

        bool SaveFrame(uint32_t frame);
        bool LoadFrame(uint32_t frame);
        uint32_t GetChecksum(uint32_t frame);

        void Reset();

    private:
        uint32_t FindSerializableObject(ISerializable* pSerializableObject);

        bool ReadAllObjects(void* pBuffer, size_t* pBytesReadOut, size_t numBytes);
        bool WriteAllObjects(void* pBuffer, size_t* pBytesWrittenOut, size_t numBytes);

        static const uint32_t kMaxSerializableObjects = 128;
        ISerializable* m_serializableObjects[kMaxSerializableObjects] = {};
        uint32_t m_serializableObjectCount = 0;

        SerializedFrameStorage m_frameStorage;
    };
}}