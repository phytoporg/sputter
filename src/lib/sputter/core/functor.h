#pragma once

#include <cstdint>
#include <sputter/core/functorstorage.h>

// Convenience wrapper around functor storage
namespace sputter { namespace core {
    class Functor
    {
    public:
        Functor();

        // Trying to avoid templates/variadics, so all args should live in pUserData
        Functor(uintptr_t functionAddress, void* pUserData = nullptr);

        void* GetUserData() const;
        void operator()() const;

        operator bool() const;

    private:
        functorstorage::FunctorHandle m_functorHandle = functorstorage::kInvalidFunctorHandle;
        void* m_pUserData = nullptr;
    };
}}