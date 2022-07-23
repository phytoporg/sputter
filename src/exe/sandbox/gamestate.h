#pragma once

#include <sputter/physics/rigidbodysubsystem.h>

struct GameState
{
    GameState(const sputter::physics::RigidBodySubsystemSettings& settings);

    sputter::physics::RigidBodySubsystem RigidBodySubsystem;
};
