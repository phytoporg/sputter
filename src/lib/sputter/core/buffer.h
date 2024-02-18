#pragma once

#include <cstdint>
#include <cstdlib>

namespace sputter { namespace core {
    class Buffer
    {
    public:
        Buffer(uint8_t* pBuffer, size_t size);

        bool WriteU8(uint8_t value);
        bool WriteSize(size_t value);
        bool WriteBytes(void* pSrc, size_t numBytesToWrite);

        void* GetData() const;

        size_t BytesRemaining() const;

    private:
        Buffer() = delete;

        uint8_t* m_pBuffer = nullptr;
        size_t m_bufferSize = 0;
        size_t m_position = 0;
    };
}}
