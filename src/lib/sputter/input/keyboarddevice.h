#pragma once

#include "inputdevice.h"
#include <vector>

namespace sputter { namespace render {
    class Window;
}}

namespace sputter { namespace input {
    class KeyboardDevice : public IInputDevice
    {
    public:
        KeyboardDevice(sputter::render::Window* pWindow);

        virtual uint32_t SampleGameInputState() const override;
        virtual uint32_t SampleGameInputState(uint32_t code) const override;

        virtual InputDeviceConnectionState GetConnectionState() const override;
        virtual void SetInputMap(InputMapEntry const* pEntries, size_t numEntries) override;

    private:
        KeyboardDevice() = delete;
        KeyboardDevice(const KeyboardDevice& other) = delete;
        KeyboardDevice& operator=(const KeyboardDevice& other) = delete;

        sputter::render::Window* m_pWindow;
        std::vector<InputMapEntry> m_inputMap;
    };
}}