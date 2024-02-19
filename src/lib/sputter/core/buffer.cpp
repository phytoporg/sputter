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

    template<typename T>
    bool Read(uint8_t* pSrc, T* pDest, size_t& positionInOut, size_t size)
    {
        constexpr size_t SizeToRead = sizeof(T);
        if ((positionInOut + SizeToRead) >= size)
        {
            return false;
        }

        T* pTSrc = reinterpret_cast<T*>(pSrc + positionInOut);
        *pDest = *pTSrc;

        positionInOut += SizeToRead;
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

bool Buffer::ReadU8(uint8_t* pValue)
{
    return Read(m_pBuffer, pValue, m_position, m_bufferSize);
}

bool Buffer::ReadSize(size_t* pValue)
{
    return Read(m_pBuffer, pValue, m_position, m_bufferSize);
}

bool Buffer::ReadBytes(void* pBuffer, size_t bytesToRead)
{
    if ((m_position + bytesToRead) >= m_bufferSize)
    {
        return false;
    }

    memcpy(pBuffer, m_pBuffer + m_position, bytesToRead);
    m_position += bytesToRead;

    return true;
}

bool Buffer::Seek(size_t position)
{
    if (position >= m_bufferSize)
    {
        return false;
    }

    m_position = position;
    return true;
}

bool Buffer::Skip(size_t bytesToSkip)
{
    if ((m_position + bytesToSkip) >= m_bufferSize)
    {
        return false;
    }

    m_position += bytesToSkip;
    return true;
}

void* Buffer::GetData() const
{
    return m_pBuffer;
}

void* Buffer::GetDataAtPosition() const
{
    return m_pBuffer + m_position;
}

size_t Buffer::BytesRemaining() const
{
    return m_bufferSize - m_position;
}

size_t Buffer::GetPosition() const
{
    return m_position;
}
