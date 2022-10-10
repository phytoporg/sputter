#pragma once

#include <cstdint>

namespace sputter { namespace assets {
    enum ResourceStorageType : uint16_t
    {
        TYPE_INVALID = 0,
        TYPE_TEXTURE,
        TYPE_SHADER,
        TYPE_FONT,
        TYPE_MAX
    };
}}
