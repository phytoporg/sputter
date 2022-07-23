#pragma once

#include <sputter/game/subsystemtype.h>
#include <array>

namespace sputter { namespace game {
    class SubsystemProvider
    {
    public:
        template<typename S>
        void AddSubsystem(S* pSubsystem)
        {
            m_subsystems[S::SubsystemId] = pSubsystem;
        }

        template<typename S>
        S* GetSubsystemByType()
        {
            return reinterpret_cast<S*>(m_subsystems[S::SubsystemId]);
        }

    private:
        std::array<void*, SubsystemType::TYPE_MAX> m_subsystems;
    };
}}
