#pragma once

#include <vector>
#include <cstdint>

#include <sputter/game/objectstorage.h>
#include <sputter/game/subsystemtype.h>

#include <sputter/core/serializable.h>

#include <sputter/physics/aabb.h>

#include "collisionresult.h"

namespace sputter { namespace game {
    class Object;
}}

namespace sputter { namespace physics {
    struct CollisionResult;

    struct Collision
    {
        static const game::SubsystemType ComponentId = game::SubsystemType::TYPE_COLLISION;

        bool Intersects(const Collision& other) const;
        bool TestIntersection(const Collision& other, CollisionResult* pCollisionResultOut) const;

        // Yet unused
        struct InitializationParameters {};

        game::objectstorage::ObjectHandle ObjectHandle = game::objectstorage::kInvalidHandle;

        static const size_t kMaxCollisionShapes = 8;
        AABB            CollisionShapes[kMaxCollisionShapes];
        size_t          NumCollisionShapes = 0;

        // Game-specified bitfields. Only objects with the same bits set can collide.
        uint32_t CollisionFlags = 0;
    };
}}