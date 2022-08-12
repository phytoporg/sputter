#pragma once

#include "inputsource.h"
#include <sputter/core/subsystem.h>
#include <vector>

namespace sputter { namespace input {
    class InputSubsystem : public sputter::core::ISubsystem<InputSource>
    {
    public:
        struct InitializationParameters {}

        virtual void Tick(math::FixedPoint dt) override;

        virtual InputSource* CreateComponent(const InputSource::InitializationParameters& params) override;
        virtual void ReleaseComponent(InputSource* pInputSource) override;

    private:
        std::vector<InputSource> m_inputSources;
    };
}}