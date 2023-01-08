#pragma once

#include <cstdint>

namespace sputter { namespace core { namespace functorstorage {
    using FunctorHandle = uint32_t;
    static const FunctorHandle kInvalidFunctorHandle = 0xFFFFFFFF;

    FunctorHandle RegisterFunctor(intptr_t functionAddress);
    void* GetFunctorFromHandle(FunctorHandle handle); 

    void LockFunctorRegistration();
}}}