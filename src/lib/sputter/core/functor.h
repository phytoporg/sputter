#pragma once

#include <cstdint>
#include <sputter/core/serializable.h>
#include <sputter/core/functorstorage.h>

// Convenience wrapper around functor storage
namespace sputter { namespace core {
    class Functor : public ISerializable
    {
    public:
        Functor();

        // ++ISerializable
        virtual bool Serialize(void* pBuffer, size_t* pBytesWrittenOut, size_t maxBytes) override;
        virtual bool Deserialize(void* pBuffer, size_t* pBytesReadOut, size_t maxBytes) override;
        virtual const char * GetName() const override { return "Functor"; };
        // --ISerializable

        // Trying to avoid templates/variadics, so all args should live in pUserData
        Functor(uintptr_t functionAddress, void* pUserData = nullptr);

        void* GetUserData() const;
        void operator()() const;

        operator bool() const;

        functorstorage::FunctorHandle GetHandle() const;

    private:
        functorstorage::FunctorHandle m_functorHandle = functorstorage::kInvalidFunctorHandle;
        void* m_pUserData = nullptr;
    };
}}