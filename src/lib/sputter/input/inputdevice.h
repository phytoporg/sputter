#pragma once

#include <vector>
#include <cstdint>
#include "inputsubsystem.h"

namespace sputter { namespace input {
    enum class InputDeviceConnectionState
    {
        Invalid,
        Connected,
        Disconnected,
        MaxValue
    };

    class IInputDevice 
    {
    public:
        virtual uint32_t SampleInputState() const = 0;
        virtual uint32_t SampleInputState(uint32_t deviceCode) const = 0;
        virtual uint32_t SampleGameInputState() const = 0;
        virtual uint32_t SampleGameInputState(uint32_t gameCode) const = 0;
        virtual InputDeviceConnectionState GetConnectionState() const = 0;
        virtual void SetInputMap(InputMapEntry const* pEntries, size_t numEntries) = 0;
        virtual uint32_t GameToDeviceInput(uint32_t gameInputCode) const = 0;
    };
}}