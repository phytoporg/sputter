#pragma once

#include <sputter/math/fixedpoint.h>
#include <sputter/math/fpvector2d.h>
#include <sputter/game/subsystemtype.h>

namespace sputter { namespace physics {
    struct RigidBody2D
    {
        static const game::SubsystemType ComponentId = game::SubsystemType::TYPE_RIGIDBODY;

        // Yet unused
        struct InitializationParameters {};

        math::FPVector2D Position;
        math::FPVector2D Velocity;
        math::FPVector2D Acceleration;

        math::FixedPoint Mass;
    };
}}

void ToString(const sputter::physics::RigidBody2D& rigidBody2D, char* pBuffer);