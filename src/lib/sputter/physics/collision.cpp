#include "collision.h"
#include "collisionshape.h"

using namespace sputter::physics;

bool Collision::Intersects(const Collision& other) const
{
    CollisionResult* pCollisionResultOut = nullptr;
    return TestIntersection(other, pCollisionResultOut);
}

bool Collision::TestIntersection(const Collision& other, CollisionResult* pCollisionResultOut) const
{
    for (size_t ai = 0; ai < NumCollisionShapes; ai++)
    {
        for (size_t bi = 0; bi < other.NumCollisionShapes; bi++)
        {
            const ICollisionShape* pOtherShape = static_cast<const ICollisionShape*>(&other.CollisionShapes[bi]);
            if (CollisionShapes[ai].Intersects(pOtherShape))
            {
                if (pCollisionResultOut)
                {
                    *pCollisionResultOut = CollisionResult(
                        this,
                        &other,
                        static_cast<const ICollisionShape*>(&CollisionShapes[ai]),
                        static_cast<const ICollisionShape*>(&other.CollisionShapes[bi]));
                }

                return true;
            }
        }
    }

    return false;
}