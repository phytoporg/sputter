#pragma once

#include <cstdint>

namespace sputter { namespace ui {
    enum class NavigationDirections : uint8_t
    {
        Invalid = 0,
        Up,
        Down,
        Left,
        Right,
        DirectionMax
    };
}}