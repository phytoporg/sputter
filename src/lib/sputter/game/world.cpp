#include "world.h"

#include <sputter/physics/rigidbodysubsystem.h>

namespace sputter { namespace game {
    SubsystemProvider* GetSubsystemProvider()
    {
        return &m_subsystemProvider;
    }
}}
