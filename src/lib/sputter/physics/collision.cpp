#include "collision.h"

#include <sputter/core/check.h>

using namespace sputter::physics;

bool Collision::Intersects(const Collision& other) const
{
    CollisionResult* pCollisionResultOut = nullptr;
    return TestIntersection(other, pCollisionResultOut);
}

bool Collision::TestIntersection(const Collision& other, CollisionResult* pCollisionResultOut) const
{
    RELEASE_CHECK(NumCollisionShapes < kMaxCollisionShapes, "Too many collision shapes!");
    for (size_t ai = 0; ai < NumCollisionShapes; ai++)
    {
        for (size_t bi = 0; bi < other.NumCollisionShapes; bi++)
        {
            if (CollisionShapes[ai].Intersects(other.CollisionShapes[bi]))
            {
                if (pCollisionResultOut)
                {
                    *pCollisionResultOut = CollisionResult(
                        this,
                        &other,
                        &CollisionShapes[ai],
                        &other.CollisionShapes[bi]);
                }

                return true;
            }
        }
    }

    return false;
}