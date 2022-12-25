#pragma once

#include <vector>
#include <cstdint>

#include <sputter/game/subsystemtype.h>
#include <sputter/physics/aabb.h>

#include "collisionresult.h"

namespace sputter { namespace game {
    class Object;
}}

namespace sputter { namespace physics {
    class ICollisionShape;
    struct CollisionResult;

    struct Collision
    {
        static const game::SubsystemType ComponentId = game::SubsystemType::TYPE_COLLISION;

        bool Intersects(const Collision& other) const;
        bool TestIntersection(const Collision& other, CollisionResult* pCollisionResultOut) const;

        // Yet unused
        struct InitializationParameters {};

        // TODO: Object ID, not a pointer
        sputter::game::Object* pObject = nullptr;

        static const size_t kMaxCollisionShapes = 8;
        AABB            CollisionShapes[kMaxCollisionShapes];
        size_t          NumCollisionShapes = 0;

        const static size_t kMaxCollisionResults = 16;
        CollisionResult CollisionsThisFrame[kMaxCollisionResults];
        size_t          NumCollisionsThisFrame = 0;

        // Game-specified bitfields. Only objects with the same bits set can collide.
        uint32_t CollisionFlags = 0;
    };
}}