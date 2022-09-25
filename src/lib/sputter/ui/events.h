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
        Activate,
        Deactivate,
        EventMax
    };

    // TODO: Is this really necessary?
    uint8_t EventCodeToParameter(EventCode event);
    EventCode ParameterToEventCode(uint8_t param);

    enum class Key : uint8_t {
        Invalid = 0,
        Up,
        Down,
        Left,
        Right,
        // Presses a button in focus for example, but I want more of a catch-all name for 
        // future scenarios, so "Activate" for now.
        Activate,
        KeyMax
    };
    void* KeyPointerToParameter(Key* key);
    bool IsDirectionKey(Key key);

    struct Event
    {
        EventCode Code = EventCode::Invalid;
        void*     pData = nullptr;;
    };
}}
