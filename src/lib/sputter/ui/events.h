#pragma once

#include <cstdint>

namespace sputter { namespace ui {
    enum class Event : uint8_t {
        EventInvalid = 0,
        EventKeyDown,
        EventKeyUp,
        EventFocusBegin,
        EventFocusEnd,
    };
}}
