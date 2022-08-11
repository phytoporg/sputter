#pragma once

#include <cstdint>

namespace sputter { namespace input {
    class IInputDevice; // TODO

    class InputSource
    {
    public: 
        InputSource(IInputDevice* pInputDevice);

        uint32_t GetInputState() const;
        uint32_t GetPreviousState() const;

        bool IsInputHeld(uint32_t gameInputCode) const;
        bool IsInputReleased(uint32_t gameInputCode) const;
        bool IsInputPressed(uint32_t gameInputCode) const;

        void Tick();

    private:
        IInputDevice* m_pInputDevice;
    };
}}