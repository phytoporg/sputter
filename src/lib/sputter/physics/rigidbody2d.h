#pragma once

#include <sputter/math/vector2d.h>
#include <sputter/game/subsystemtype.h>

namespace sputter { namespace physics {
    struct RigidBody2D
    {
        static const game::SubsystemType ComponentId =
            game::SubsystemType::TYPE_RIGIDBODY;

        math::Vector2D Position;
        math::Vector2D Velocity;
        math::Vector2D Acceleration;

        float Mass;
    };
}}
