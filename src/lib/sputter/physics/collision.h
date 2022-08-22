#pragma once

#include <vector>
#include <cstdint>
#include <sputter/game/subsystemtype.h>

namespace sputter { namespace game {
    class Object;
}}

namespace sputter { namespace physics {
    class ICollisionShape;
    struct CollisionResult;

    struct Collision
    {
        static const game::SubsystemType ComponentId = game::SubsystemType::TYPE_COLLISION;

        Collision() { CollisionShapes.reserve(100); }

        bool Intersects(const Collision& other) const;
        bool TestIntersection(const Collision& other, CollisionResult* pCollisionResultOut) const;

        // Yet unused
        struct InitializationParameters {};

        // TODO: Object ID, not a pointer
        sputter::game::Object* pObject = nullptr;

        // TODO: Fixed vector
        std::vector<ICollisionShape*> CollisionShapes;

        // TODO: Fixed vector
        std::vector<CollisionResult> CollisionsThisFrame;

        // Game-specified bitfields. Only objects with the same bits set can collide.
        uint32_t CollisionFlags = 0;
    };
}}