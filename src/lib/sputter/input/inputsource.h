#pragma once

#include <cstdint>
#include <vector>
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
        virtual ~InputSource();

        IInputDevice* GetInputDevice();

        static const uint32_t kCurrentFrame = 0xFFFFFFFF;
        uint32_t GetInputState(uint32_t frame = kCurrentFrame) const;
        uint32_t GetPreviousState(uint32_t frame = kCurrentFrame) const;

        bool IsInputHeld(uint32_t gameInputCode, uint32_t frame = kCurrentFrame) const;
        bool IsInputReleased(uint32_t gameInputCode, uint32_t frame = kCurrentFrame) const;
        bool IsInputPressed(uint32_t gameInputCode, uint32_t frame = kCurrentFrame) const;

        void Tick();

        void SetFrame(uint32_t frame);

    private:
        IInputDevice* m_pInputDevice;

        // Straight linear buffers for now. Should make recordings nice and trivial!
        const uint32_t kMaxNumInputFrames = 60 * 300; // Five minutes of gameplay
        std::vector<uint32_t> m_inputStateBuffer;

        uint32_t CurrentFrame = 0;
    };
}}