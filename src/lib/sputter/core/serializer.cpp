#include "serializer.h"
#include "serializable.h"
#include "check.h"

using namespace sputter;
using namespace sputter::core;

static const uint32_t kInvalidIndex = 0xFFFFFFFF;

bool Serializer::RegisterSerializableObject(ISerializable* pSerializableObject)
{
    if (FindSerializableObject(pSerializableObject) != kInvalidIndex)
    {
        // Already registered.
        return true;
    }

    RELEASE_CHECK(pSerializableObject, "Attempting to register null object pointer for serialization");

    if (m_serializableObjectCount >= kMaxSerializableObjects)
    {
        // Hit the max! Don't register any additional objects.
        return false;
    }
    
    // Find the first empty slot and place the object there.
    for (uint32_t i = 0; i < kMaxSerializableObjects; ++i)
    {
        if (m_serializableObjects[i] == nullptr)
        {
            m_serializableObjects[i] = pSerializableObject;
            m_serializableObjectCount++;
            return true;
        }
    }

    // Could not find a free slot!
    return false;
}

bool Serializer::UnregisterSerializableObject(ISerializable* pSerializableObject)
{
    RELEASE_CHECK(pSerializableObject, "Attempting to unregister null object pointer for serialization");

    const uint32_t ObjectIndex = FindSerializableObject(pSerializableObject);
    if (ObjectIndex == kInvalidIndex)
    {
        return false;
    }
    RELEASE_CHECK(m_serializableObjectCount > 0, "Serializer object count is zero or less, but we found an object.");
    
    m_serializableObjects[ObjectIndex] = nullptr;
    m_serializableObjectCount--;
    return true;
}

bool Serializer::ReadAllObjects(void* pBuffer, size_t* pBytesReadOut, size_t numBytes)
{
    size_t totalBytesRead = 0;
    uint32_t numObjectsRead = 0;
    for (uint32_t i = 0; i < kMaxSerializableObjects; ++i)
    {
        ISerializable* pCurrentSerializableObject = m_serializableObjects[i];
        if (!pCurrentSerializableObject)
        {
            // Skip invalid entries.
            continue;
        }

        void* pCurrentReadBase = static_cast<uint8_t*>(pBuffer) + totalBytesRead;
        size_t bytesRead = 0;
        RELEASE_CHECK(numBytes >= totalBytesRead, "Serializer has read beyond available buffer size");
        const size_t BytesRemaining = numBytes - totalBytesRead;
        const bool Success = pCurrentSerializableObject->Deserialize(pCurrentReadBase, &bytesRead, BytesRemaining);
        if (!Success)
        {
            // Bail if a single object failed to read.
            return false;
        }

        RELEASE_CHECK(bytesRead <= BytesRemaining, "Serializable object read more bytes than expected");
        totalBytesRead += bytesRead;
        RELEASE_CHECK(totalBytesRead <= numBytes, "Serializer has read more bytes than expected");
         
        ++numObjectsRead;
        if (numObjectsRead == m_serializableObjectCount)
        {
            break;
        }
    }
    RELEASE_CHECK(numObjectsRead == m_serializableObjectCount, "Serializer did not read expected number of objects")

    *pBytesReadOut = totalBytesRead;
    return true;
}

bool Serializer::WriteAllObjects(void* pBuffer, size_t* pBytesWrittenOut, size_t numBytes)
{
    size_t totalBytesWritten = 0;
    uint32_t numObjectsWritten = 0;
    for (uint32_t i = 0; i < kMaxSerializableObjects; ++i)
    {
        ISerializable* pCurrentSerializableObject = m_serializableObjects[i];
        if (!pCurrentSerializableObject)
        {
            // Skip invalid entries.
            continue;
        }

        void* pCurrentWriteBase = static_cast<uint8_t*>(pBuffer) + totalBytesWritten;
        size_t bytesWritten = 0;
        RELEASE_CHECK(numBytes >= totalBytesWritten, "Serializer has written beyond available buffer size");
        const size_t BytesRemaining = numBytes - totalBytesWritten;
        const bool Success = pCurrentSerializableObject->Serialize(pCurrentWriteBase, &bytesWritten, BytesRemaining);
        if (!Success)
        {
            // Bail if a single object failed to write.
            return false;
        }

        RELEASE_CHECK(bytesWritten <= BytesRemaining, "Serializable object wrote more bytes than expected");
        totalBytesWritten += bytesWritten;
        RELEASE_CHECK(totalBytesWritten <= numBytes, "Serializer has written more bytes than expected");

        ++numObjectsWritten;
        if (numObjectsWritten == m_serializableObjectCount)
        {
            break;
        }
    }
    RELEASE_CHECK(numObjectsWritten == m_serializableObjectCount, "Serializer did not write expected number of objects")

    *pBytesWrittenOut = totalBytesWritten;
    return true;
}

uint32_t Serializer::FindSerializableObject(ISerializable* pSerializableObject)
{
    uint32_t numObjectsFound = 0;
    for (uint32_t i = 0; i < kMaxSerializableObjects; ++i)
    {
        if (m_serializableObjects[i] == pSerializableObject)
        {
            return i;
        }
        else if (m_serializableObjects[i])
        {
            ++numObjectsFound;
            if (numObjectsFound == m_serializableObjectCount)
            {
                // We've seen 'em all. Early exit.
                break;
            }
        }
    }

    return kInvalidIndex;
}