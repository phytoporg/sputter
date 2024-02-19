#pragma once

#include <cstdint>
#include <cstdlib>

namespace sputter { namespace core {
    class Buffer
    {
    public:
        Buffer(uint8_t* pBuffer, size_t capacity);

        bool WriteU8(uint8_t value);
        bool WriteSize(size_t value);
        bool WriteBytes(void* pSrc, size_t numBytesToWrite);

        bool ReadU8(uint8_t* pValue);
        bool ReadSize(size_t* pValue);
        bool ReadBytes(void* pBuffer, size_t bytesToRead);

        bool Seek(size_t position);
        bool Skip(size_t bytesToSkip);

        void* GetData() const;
        void* GetDataAtPosition() const;
        size_t BytesRemaining() const;
        size_t GetPosition() const;

    private:
        Buffer() = delete;

        uint8_t* m_pBuffer = nullptr;
        size_t m_bufferSize = 0;
        size_t m_position = 0;
    };
}}
