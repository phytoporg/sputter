#include "functor.h"
#include "functorstorage.h"
#include "check.h"

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