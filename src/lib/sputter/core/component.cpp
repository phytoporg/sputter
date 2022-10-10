#include "component.h"

namespace sputter { namespace core {
    ComponentHandle GetStorageTypeFromHandle(ComponentHandle handle)
    {
        return reinterpret_cast<ComponentHandle>((handle >> 16) & 0xFFFF);
    }

    uint16_t GetComponentIndexFromHandle(ComponentHandle handle)
    {
        return static_cast<uint16_t>(handle & 0xFFFF);
    }
}}