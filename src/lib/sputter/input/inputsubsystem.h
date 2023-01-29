#pragma once

#include "inputsource.h"
#include <sputter/core/subsystem.h>
#include <sputter/core/component.h>
#include <sputter/game/subsystemtype.h>
#include <vector>

namespace sputter { namespace render {
    class Window;
}}

namespace sputter { namespace input {
    enum class DeviceType
    {
        Invalid = 0,
        None,
        KeyboardInputDevice,
        MaxDeviceType
    };

    struct InputMapEntry
    {
        uint32_t DeviceInputCode;
        uint32_t GameInputCode;
    };

    struct InputSubsystemSettings
    {
        // For keyboards
        sputter::render::Window* pWindow;

        // 2 players max for now?
        DeviceType PlayerDevices[2];

        InputMapEntry const* pInputMapEntryArrays[2];
        size_t pNumInputMapEntries[2];
    };

    class InputSubsystem : public sputter::core::ISubsystem<InputSource>
    {
    public:
        static const game::SubsystemType SubsystemId = 
            game::SubsystemType::TYPE_INPUTSOURCE;

        InputSubsystem(const InputSubsystemSettings& settings);

        virtual void Tick(math::FixedPoint dt) override;

        virtual InputSource* CreateComponent(const InputSource::InitializationParameters& params) override;
        virtual void ReleaseComponent(InputSource* pInputSource) override;

        core::ComponentHandle GetComponentHandle(InputSource* pInputSource) const;
        InputSource* GetComponentFromHandle(core::ComponentHandle handle);

        const InputSource* GetInputSource(uint8_t playerIndex) const;

        void SetFrame(uint32_t frame);

    private:
        std::vector<InputSource> m_inputSources;
    };
}}