#include "functor.h"
#include "functorstorage.h"
#include "check.h"
#include <sputter/log/framestatelogger.h>

using namespace sputter;
using namespace sputter::core;

Functor::Functor()
    : m_functorHandle(functorstorage::kInvalidFunctorHandle), m_pUserData(nullptr)
{}

// Trying to avoid templates/variadics, so all args should live in pUserData
Functor::Functor(uintptr_t functionAddress, void* pUserData)
    : m_pUserData(pUserData)
{
    m_functorHandle = functorstorage::RegisterFunctor(functionAddress);
    RELEASE_CHECK(m_functorHandle != functorstorage::kInvalidFunctorHandle, "Failed to create functor");
}

bool Functor::Serialize(void *pBuffer, size_t *pBytesWrittenOut, size_t maxBytes)
{
    WRITE_PROPERTY(m_functorHandle, pBuffer, *pBytesWrittenOut, maxBytes);
    *pBytesWrittenOut += sizeof(m_functorHandle);
    return true;
}

bool Functor::Deserialize(void *pBuffer, size_t *pBytesReadOut, size_t maxBytes)
{
    READ(m_functorHandle, pBuffer, *pBytesReadOut, maxBytes);
    *pBytesReadOut += sizeof(m_functorHandle);
    return true;
}

void* Functor::GetUserData() const
{
    return m_pUserData;
}

void Functor::operator()() const
{
    RELEASE_CHECK(*this, "Attempting to invoke invalid functor");

    void (*pfnAddress)(void*) = reinterpret_cast<void(*)(void*)>(functorstorage::GetFunctorFromHandle(m_functorHandle));
    RELEASE_CHECK(pfnAddress, "Could not resolve functor handle");

    pfnAddress(m_pUserData);
}


Functor::operator bool() const
{
    return m_functorHandle != functorstorage::kInvalidFunctorHandle;
}

functorstorage::FunctorHandle Functor::GetHandle() const
{
    return m_functorHandle;
}
