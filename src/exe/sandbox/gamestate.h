#pragma once

#include <sputter/physics/rigidbodysubsystem.h>
#include <sputter/render/camera.h>
#include "testobject.h"
#include "charactercube.h"

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

    sputter::render::Camera Camera;
    TestObject              MainShip;
    CharacterCube           MainCube;
};
