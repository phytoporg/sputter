#include "collision.h"
#include "collisionshape.h"

using namespace sputter::physics;

bool Collision::Intersects(const Collision& other) const
{
    for (size_t ai = 0; ai < CollisionShapes.size(); ai++)
    {
        for (size_t bi = 0; bi < other.CollisionShapes.size(); bi++)
        {
            if (CollisionShapes[ai]->Intersects(other.CollisionShapes[bi]))
            {
                return true;
            }
        }
    }

    return false;
}