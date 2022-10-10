#pragma once

#include <cstdint>

// Should be in ECS rather than "core?"

namespace sputter { namespace core {
    using ComponentHandle = uint32_t;
    static const ComponentHandle kInvalidComponentHandle = 0xFFFFFFFF;

    template<typename ComponentType>
    ComponentHandle CreateComponentHandle(uint16_t componentIndex)
    {
        return ((static_cast<uint16_t>(ComponentType::ComponentId) & 0xFFFF) << 16) | componentIndex;
    }

    ComponentHandle GetStorageTypeFromHandle(ComponentHandle handle);
    uint16_t GetComponentIndexFromHandle(ComponentHandle handle);
}}