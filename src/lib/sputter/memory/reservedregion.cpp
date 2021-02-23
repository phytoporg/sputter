#include <cstring>

#include "reservedregion.h"

namespace sputter { namespace memory {
    ReservedRegion::ReservedRegion(size_t regionSize)
        : m_spRegionMemory(new uint8_t[regionSize]),
          m_regionSize(regionSize)
    {
        // 0xCC for more conspicuous uninitialized memory while debugging.
        std::memset(m_spRegionMemory.get(), 0xCC, regionSize);
    }

    uint8_t* ReservedRegion::GetRegionBase()
    {
        return m_spRegionMemory.get();
    }

    size_t ReservedRegion::GetRegionSize() const
    {
        return m_regionSize;
    }
}}
