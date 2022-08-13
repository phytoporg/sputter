#pragma once

#include <cstdint>
#include <sputter/game/subsystemtype.h>

namespace sputter { namespace input {
    class IInputDevice; // TODO

    class InputSource
    {
    public: 
        static const game::SubsystemType ComponentId = game::SubsystemType::TYPE_INPUTSOURCE;

        struct InitializationParameters
        {
            uint32_t PlayerId;
        };

        InputSource(IInputDevice* pInputDevice);

        IInputDevice* GetInputDevice();

        uint32_t GetInputState() const;
        uint32_t GetPreviousState() const;

        bool IsInputHeld(uint32_t gameInputCode) const;
        bool IsInputReleased(uint32_t gameInputCode) const;
        bool IsInputPressed(uint32_t gameInputCode) const;

        void Tick();

    private:
        IInputDevice* m_pInputDevice;

        uint32_t m_previousInput;
        uint32_t m_currentInput;
    };
}}