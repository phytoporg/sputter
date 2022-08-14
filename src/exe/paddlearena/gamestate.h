#pragma once

#include <sputter/physics/rigidbodysubsystem.h>
#include <sputter/render/camera.h>
#include "paddle.h"
#include "stage.h"

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
    Paddle           Player1Paddle;
    Paddle           Player2Paddle;
    Stage            Arena;
};
