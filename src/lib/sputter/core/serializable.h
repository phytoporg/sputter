#pragma once

// Interface for serializable objects

#include <cstdint>
#include <cstddef>

namespace sputter { namespace core { 
    class ISerializable 
    {
    public:
        virtual bool Serialize(void* pBuffer, size_t* pBytesWrittenOut, size_t maxBytes) = 0;
        virtual bool Deserialize(void* pBuffer, size_t* pBytesReadOut, size_t maxBytes) = 0;

    protected:
        // Helpers
        bool Write(void* pSource, void* pDestination, size_t bytesToWrite, size_t maxBytes);
        bool Read(void* pDestination, void* pSource, size_t bytesToRead, size_t maxBytes);
    };
}}

#define WRITE_ARRAY(toWrite, pDestination, offset, maxBytes) \
    if (!Write(toWrite, (char*)pDestination + offset, sizeof(toWrite), maxBytes - offset)) { return false; }

#define WRITE(toWrite, pDestination, offset, maxBytes) \
    if (!Write(&(toWrite), (char*)pDestination + offset, sizeof(toWrite), maxBytes - offset)) { return false; }

#define READ(toRead, pSource, offset, maxBytes) \
    if (!Read(&(toRead), (char*)pSource + offset, sizeof(toRead), maxBytes - offset)) { return false; }

#define READ_ARRAY(toRead, pSource, offset, maxBytes) \
    if (!Read(toRead, (char*)pSource + offset, sizeof(toRead), maxBytes - offset)) { return false; }