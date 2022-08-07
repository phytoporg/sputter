#pragma once

#include <sputter/physics/rigidbodysubsystem.h>
#include "testobject.h"

namespace sputter 
{
    namespace assets 
    {
        class AssetStorageProvider;
    }

    namespace game 
    {
        class SubsystemProvider;
    }
}

struct GameState
{
    GameState(
        sputter::assets::AssetStorageProvider* pStorageProvider,
        sputter::game::SubsystemProvider* pSubsystemProvider
        );

    TestObject   MainShip;
};
