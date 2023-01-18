#pragma once

// The serializer tracks all objects needing serialization, and 
// serializes or deserializes their state into or from a contiguous
// buffer, respectively.

#include <cstdint>
#include <cstddef>

namespace sputter { namespace core {
    class ISerializable;

    class Serializer
    {
    public:
        bool RegisterSerializableObject(ISerializable* pSerializableObject);
        bool UnregisterSerializableObject(ISerializable* pSerializableObject);

        bool ReadAllObjects(void* pBuffer, size_t* pBytesReadOut, size_t numBytes);
        bool WriteAllObjects(void* pBuffer, size_t* pBytesWrittenOut, size_t numBytes);

    private:
        uint32_t FindSerializableObject(ISerializable* pSerializableObject);

        static const uint32_t kMaxSerializableObjects = 128;
        ISerializable* m_serializableObjects[kMaxSerializableObjects] = {};
        uint32_t m_serializableObjectCount;
    };
}}