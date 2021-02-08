#pragma once

#include <sputter/math/vector2d.h>

namespace sputter { namespace physics {
    struct RigidBody2D
    {
        math::Vector2D Position;
        math::Vector2D Velocity;
        math::Vector2D Acceleration;

        float Mass;
    };
}}
