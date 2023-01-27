#pragma once

namespace sputter { namespace physics {
    class Collision;
    struct CollisionResult
    {
        CollisionResult() {}
        CollisionResult(
            const Collision* pA,
            const Collision* pB,
            const AABB* pShapeA,
            const AABB* pShapeB);

        const Collision* pCollisionA = nullptr;
        const Collision* pCollisionB = nullptr;
        const AABB* pCollisionShapeA = nullptr;
        const AABB* pCollisionShapeB = nullptr;
    };
}}