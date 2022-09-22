#pragma once

#include <cstdint>

namespace sputter { namespace ui {
    enum class EventCode : uint8_t {
        Invalid = 0,
        KeyDown,
        KeyHeld,
        KeyUp,
        FocusBegin,
        FocusEnd,
        ChildAdded,
        ChildRemoved,
        EventMax
    };

    // TODO: Is this really necessary?
    uint8_t EventCodeToParameter(EventCode event);
    EventCode ParameterToEventCode(uint8_t param);

    enum class Key : uint8_t {
        Invalid = 0,
        Up,
        Down,
        KeyMax
    };
    void* KeyPointerToParameter(Key* key);

    struct Event
    {
        EventCode Code = EventCode::Invalid;
        void*     pData = nullptr;;
    };
}}
