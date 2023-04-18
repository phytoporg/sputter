#include "collision.h"

#include <sputter/log/framestatelogger.h>
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

bool Collision::Serialize(void *pBuffer, size_t *pBytesWrittenOut, size_t maxBytes)
{
    WRITE_PROPERTY(NumCollisionShapes, pBuffer, *pBytesWrittenOut, maxBytes);
    *pBytesWrittenOut += sizeof(NumCollisionShapes);

    WRITE_ARRAY_PROPERTY(CollisionShapes, NumCollisionShapes, pBuffer, *pBytesWrittenOut, maxBytes);
    *pBytesWrittenOut += sizeof(CollisionShapes[0]) * NumCollisionShapes;

    WRITE_PROPERTY(CollisionFlags, pBuffer, *pBytesWrittenOut, maxBytes);
    *pBytesWrittenOut += sizeof(CollisionFlags);

    return true;
}

bool Collision::Deserialize(void *pBuffer, size_t *pBytesReadOut, size_t maxBytes)
{
    READ(NumCollisionShapes, pBuffer, *pBytesReadOut, maxBytes);
    *pBytesReadOut += sizeof(NumCollisionShapes);

    READ_ARRAY(CollisionShapes, NumCollisionShapes, pBuffer, *pBytesReadOut, maxBytes);
    *pBytesReadOut += sizeof(CollisionShapes[0]) * NumCollisionShapes;

    READ(CollisionFlags, pBuffer, *pBytesReadOut, maxBytes);
    *pBytesReadOut += sizeof(CollisionFlags);

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
