#include "resourcestorage.h"

namespace sputter { namespace assets {
    ResourceStorageType GetStorageTypeFromHandle(ResourceHandle handle)
    {
        return static_cast<ResourceStorageType>((handle >> 16) & 0xFFFF);
    }

    uint16_t GetResourceIndexFromHandle(ResourceHandle handle)
    {
        return static_cast<uint16_t>(handle & 0xFFFF);
    }
}}