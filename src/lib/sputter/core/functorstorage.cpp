#include "functorstorage.h"
#include "check.h"

using namespace sputter;
using namespace sputter::core;
using namespace sputter::core::functorstorage;

static const uint32_t kMaxFunctorCount = 32;

static intptr_t s_functorTable[kMaxFunctorCount] = {};
static uint32_t s_functorCount = 0;

static bool s_isRegistrationLocked = false;

static FunctorHandle IndexToHandle(uint32_t index) 
{
    return static_cast<FunctorHandle>(index);
}

FunctorHandle RegisterFunctor(intptr_t functionAddress)
{
    RELEASE_CHECK(!s_isRegistrationLocked, "Attempting to register functor, but functor registration is locked");
    RELEASE_CHECK(s_functorCount < kMaxFunctorCount, "Cannot register any more functors");

    // If it's already registered, just return the existing handle
    for (uint32_t i = 0; i < s_functorCount; ++i)
    {
        if (s_functorTable[i] == functionAddress)
        {
            return IndexToHandle(i);
        }
    }

    s_functorTable[s_functorCount] = functionAddress;
    ++s_functorCount;
    return IndexToHandle(s_functorCount - 1);
}

void* GetFunctorFromHandle(FunctorHandle handle)
{
    const uint32_t Index = static_cast<uint32_t>(handle);
    RELEASE_CHECK(Index < s_functorCount, "Functor handle is invalid - resolved index is out of bounds");

    return s_functorTable[Index];
}

void LockFunctorRegistration()
{
    s_isRegistrationLocked = true;
}
