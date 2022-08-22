#include "collision.h"
#include "collisionshape.h"
#include "collisionsubsystem.h"

using namespace sputter::physics;

bool Collision::Intersects(const Collision& other) const
{
    CollisionResult* pCollisionResultOut = nullptr;
    return TestIntersection(other, pCollisionResultOut);
}

bool Collision::TestIntersection(const Collision& other, CollisionResult* pCollisionResultOut) const
{
    for (size_t ai = 0; ai < CollisionShapes.size(); ai++)
    {
        for (size_t bi = 0; bi < other.CollisionShapes.size(); bi++)
        {
            if (CollisionShapes[ai]->Intersects(other.CollisionShapes[bi]))
            {
                if (pCollisionResultOut)
                {
                    *pCollisionResultOut = CollisionResult{this, &other, CollisionShapes[ai], other.CollisionShapes[bi]};
                }

                return true;
            }
        }
    }

    return false;
}