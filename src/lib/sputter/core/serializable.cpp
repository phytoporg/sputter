#include "serializable.h"
#include <cstring>

using namespace sputter;
using namespace sputter::core;

bool ISerializable::Write(void* pSource, void* pDestination, size_t bytesToWrite, size_t maxBytes)
{
    if (bytesToWrite > maxBytes)
    {
        return false;
    }

    memcpy(pDestination, pSource, bytesToWrite);
    return true;
}

bool ISerializable::Read(void* pDestination, void* pSource, size_t bytesToRead, size_t maxBytes)
{
    if (bytesToRead > maxBytes)
    {
        return false;
    }

    memcpy(pDestination, pSource, bytesToRead);
    return true;
}