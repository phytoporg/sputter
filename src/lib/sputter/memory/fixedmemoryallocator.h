#pragma once

#include <sputter/system/system.h>
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
                LOG(WARNING) << "Create() - Out of memory: " << m_allocatorName;
                return nullptr;
            }

            T* pCreated = new (m_pNext) T(std::forward<Args>(args)...);
            if (pCreated)
            {
                m_pNext += sizeof(T);
            }

            return pCreated;
        }

        template<typename T>
        T* ReserveNext(size_t count)
        {
            if ((m_pNext + sizeof(T) * count) > (m_pBase + m_regionSize))
            {
                LOG(WARNING) << "ReserveNext() - Out of memory: " 
                             << m_allocatorName;
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

        uint8_t* GetBase();

    private:
        uint8_t* m_pBase;
        uint8_t* m_pNext;
        size_t   m_regionSize;

        std::string m_allocatorName;
    };
}}