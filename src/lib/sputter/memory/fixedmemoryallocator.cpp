#include "fixedmemoryallocator.h"

#include <sputter/log/log.h>

namespace sputter { namespace memory {
    FixedMemoryAllocator::FixedMemoryAllocator(const std::string& allocatorName)
        : m_allocatorName(allocatorName),
        m_pBase(nullptr),
        m_pNext(nullptr),
        m_regionSize(0)
    {}

    FixedMemoryAllocator::FixedMemoryAllocator(
        const std::string& allocatorName,
        uint8_t* pBase,
        size_t regionSize
        ) : m_allocatorName(allocatorName),
            m_pBase(pBase),
            m_pNext(m_pBase),
            m_regionSize(regionSize) 
    {}

    bool FixedMemoryAllocator::ReserveNext(
        const std::string& allocatorName, 
        size_t size,
        FixedMemoryAllocator* pOut)
    {
        if ((m_pNext + size) > (m_pBase + m_regionSize))
        {
            RELEASE_LOGLINE_WARNING(LOG_DEFAULT, "ReserveNext FMA - Out of memory %s", allocatorName.c_str());
            return false;
        }

        *pOut = FixedMemoryAllocator(allocatorName, m_pNext, size);
        m_pNext += size;

        return true;
    }

    void* FixedMemoryAllocator::ReserveNext(size_t size)
    {
        if ((m_pNext + size) > (m_pBase + m_regionSize))
        {
            RELEASE_LOGLINE_WARNING(LOG_DEFAULT, "ReserveNext FMA - Out of memory %s");
            return nullptr;
        }

        uint8_t* pReserved = m_pNext;
        m_pNext += size;
        return static_cast<void*>(pReserved);

    }

    uint8_t* FixedMemoryAllocator::GetBase()
    {
        return m_pBase;
    }
}}
