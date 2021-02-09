#pragma once

#include "subsystemprovider.h"

#include <sputter/physics/rigidbodysubsystem.h>

#include <memory>

namespace sputter { namespace game {
    class World
    {
    public:
        virtual void Tick(float dt) = 0;

        SubsystemProvider* GetSubsystemProvider();

    private:
        SubsystemProvider m_subsystemProvider;
    };
}}
