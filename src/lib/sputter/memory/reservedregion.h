#pragma once

#include <memory>

namespace sputter { namespace memory {
    class ReservedRegion
    {
    public:
        ReservedRegion(size_t regionSize);

        uint8_t* GetRegionBase();
        size_t   GetRegionSize() const;

    private:
        std::unique_ptr<uint8_t[]> m_spRegionMemory;
        size_t m_regionSize;
    };
}}
