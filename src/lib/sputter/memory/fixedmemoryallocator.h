#pragma once

#include <sputter/system/system.h>
#include <sputter/log/log.h>
#include <sputter/core/check.h>
#include <string>

namespace sputter { namespace memory {
    class FixedMemoryAllocator
    {
    public:
        FixedMemoryAllocator(const std::string& allocatorName);

        FixedMemoryAllocator(
            const std::string& allocatorName,
            uint8_t* pBase,
            size_t regionSize);

        template <typename T, typename... Args>
        T* Create(Args&&... args) 
        {
            if ((m_pNext + sizeof(T)) > (m_pBase + m_regionSize))
            {
                RELEASE_LOG_WARNING(LOG_DEFAULT, "Create() - Out of memory: %s", m_allocatorName.c_str());
                return nullptr;
            }

            // Increment the address prior to invoking placement new, in case the constructor we're
            // calling allocates additional data on this allocator.
            void* pCreationAddress = m_pNext;
            m_pNext += sizeof(T);

            T* pCreated = new (pCreationAddress) T(std::forward<Args>(args)...);
            RELEASE_CHECK(pCreated, "Placement new failed in fixed allocator!");
            m_pNext += sizeof(T);

            return pCreated;
        }

        template<typename T>
        T* ReserveNext(size_t count)
        {
            if ((m_pNext + sizeof(T) * count) > (m_pBase + m_regionSize))
            {
                RELEASE_LOG_WARNING(LOG_DEFAULT, "ReserveNext() - Out of memory: %s", m_allocatorName.c_str());
                return nullptr;
            }

            T* returnValue = reinterpret_cast<T*>(m_pNext);
            m_pNext += sizeof(T) * count;

            return returnValue;
        }

        bool ReserveNext(
            const std::string& allocatorName, 
            size_t size,
            FixedMemoryAllocator* pOut);

        void* ReserveNext(size_t size);

        uint8_t* GetBase();

    private:
        uint8_t* m_pBase;
        uint8_t* m_pNext;
        size_t   m_regionSize;

        std::string m_allocatorName;
    };
}}
