#pragma once

namespace sputter { namespace physics {
    class Collision;
    class ICollisionShape;
    struct CollisionResult
    {
        CollisionResult() {}
        CollisionResult(
            const Collision* pA,
            const Collision* pB,
            const ICollisionShape* pShapeA,
            const ICollisionShape* pShapeB);

        const Collision* pCollisionA = nullptr;
        const Collision* pCollisionB = nullptr;
        const ICollisionShape* pCollisionShapeA = nullptr;
        const ICollisionShape* pCollisionShapeB = nullptr;
    };
}}