#pragma once

#include <cstdint>

namespace sputter { namespace ui {
    enum class Event : uint8_t {
        Invalid = 0,
        KeyDown,
        KeyUp,
        FocusBegin,
        FocusEnd,
        EventMax
    };

    enum class Key : uint8_t {
        Invalid = 0,
        Up,
        Down
    };
}}
