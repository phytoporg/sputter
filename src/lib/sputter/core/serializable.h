#pragma once

// Interface for serializable objects

#include <cstdint>

namespace sputter { namespace core { 
    class ISerializable 
    {
    public:
        virtual bool Serialize(void* pBuffer, size_t* pBytesWrittenOut, size_t maxBytes) = 0;
        virtual bool Deserialize(void* pBuffer, size_t* pBytesReadOut, size_t maxBytes) = 0;
    };
}}