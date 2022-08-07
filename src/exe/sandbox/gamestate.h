#pragma once

#include <sputter/physics/rigidbodysubsystem.h>
#include "testobject.h"

namespace sputter 
{
    namespace game 
    {
        class SubsystemProvider;
    }
}

struct GameState
{
    GameState(sputter::game::SubsystemProvider* pSubsystemProvider);

    TestObject   MainShip;
};
