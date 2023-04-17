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

// game::objectstorage::ObjectHandle ObjectHandle = game::objectstorage::kInvalidHandle;
//
// static const size_t kMaxCollisionShapes = 8;
// AABB            CollisionShapes[kMaxCollisionShapes];
// size_t          NumCollisionShapes = 0;
//
// // Game-specified bitfields. Only objects with the same bits set can collide.
// uint32_t CollisionFlags = 0;

void ToString(const Collision &collision, char *pBuffer)
{
    sprintf(
        pBuffer,
        "ObjectHandle: %u, CollisionShapes: TODO, NumCollisionShapes: %llu, CollisionFlags: 0x%08X",
        collision.ObjectHandle, collision.NumCollisionShapes, collision.CollisionFlags
    );
}
