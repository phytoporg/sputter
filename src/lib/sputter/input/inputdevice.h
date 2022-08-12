#pragma once

#include <vector>
#include <cstdint>

namespace sputter { namespace input {
    enum class InputDeviceConnectionState
    {
        Invalid,
        Connected,
        Disconnected,
        MaxValue
    };

    class InputDevice 
    {
    public:
        virtual uint32_t SampleInputState(uint32_t code) const = 0;
        virtual InputDeviceConnectionState GetConnectionState() const = 0;
    };
}}