#pragma once

#include <sputter/physics/rigidbodysubsystem.h>
#include <sputter/render/camera.h>
#include "paddle.h"
#include "stage.h"
#include "ball.h"

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
    Ball             TheBall;
    Paddle           Player1Paddle;
    Paddle           Player2Paddle;
    Stage            Arena;
};
