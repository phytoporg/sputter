#include "buffer.h"
#include <cstring>

using namespace sputter;
using namespace sputter::core;

namespace 
{
    template<typename T>
    bool Write(uint8_t* pDest, T src, size_t& positionInOut, size_t size)
    {
        constexpr size_t SizeToWrite = sizeof(T);
        if ((positionInOut + SizeToWrite) >= size)
        {
            return false;
        }

        T* pTDest = reinterpret_cast<T*>(pDest + positionInOut);
        *pTDest = src;

        positionInOut += SizeToWrite;
        return true;
    }
}

Buffer::Buffer(uint8_t* pBuffer, size_t size)
    : m_pBuffer(pBuffer), m_bufferSize(size)
{}

bool Buffer::WriteU8(uint8_t value)
{
    return Write(m_pBuffer, value, m_position, m_bufferSize);
}

bool Buffer::WriteSize(size_t value)
{
    return Write(m_pBuffer, value, m_position, m_bufferSize);
}

bool Buffer::WriteBytes(void* pSrc, size_t numBytesToWrite)
{
    if ((m_position + numBytesToWrite) >= m_bufferSize)
    {
        return false;
    }

    memcpy(m_pBuffer + m_position, pSrc, numBytesToWrite);
    m_position += numBytesToWrite;

    return true;
}

void* Buffer::GetData() const
{
    return m_pBuffer;
}

size_t Buffer::BytesRemaining() const
{
    return m_bufferSize - m_position;
}
