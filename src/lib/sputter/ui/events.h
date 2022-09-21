#pragma once

#include <cstdint>

namespace sputter { namespace ui {
    enum class Event : uint8_t {
        Invalid = 0,
        KeyDown,
        KeyUp,
        FocusBegin,
        FocusEnd,
        ChildAdded,
        ChildRemoved,
        EventMax
    };
    // TODO: Is this really necessary?
    uint8_t EventToParameter(Event event);

    enum class Key : uint8_t {
        Invalid = 0,
        Up,
        Down
    };
    void* KeyPointerToParameter(Key* key);
}}
