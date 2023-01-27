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

bool Collision::Serialize(void* pBuffer, size_t* pBytesWrittenOut, size_t maxBytes)
{
    WRITE(ObjectHandle, pBuffer, *pBytesWrittenOut, maxBytes);
    *pBytesWrittenOut += sizeof(ObjectHandle);

    WRITE_ARRAY(CollisionShapes, pBuffer, *pBytesWrittenOut, maxBytes);
    *pBytesWrittenOut += sizeof(CollisionShapes);

    WRITE(NumCollisionShapes, pBuffer, *pBytesWrittenOut, maxBytes);
    *pBytesWrittenOut += sizeof(NumCollisionShapes);

    WRITE(CollisionFlags, pBuffer, *pBytesWrittenOut, maxBytes);
    *pBytesWrittenOut += sizeof(CollisionFlags);

    return true;
}

bool Collision::Deserialize(void* pBuffer, size_t* pBytesReadOut, size_t maxBytes)
{
    READ(ObjectHandle, pBuffer, *pBytesReadOut, maxBytes);
    *pBytesReadOut += sizeof(ObjectHandle);

    READ_ARRAY(CollisionShapes, pBuffer, *pBytesReadOut, maxBytes);
    *pBytesReadOut += sizeof(CollisionShapes);

    READ(NumCollisionShapes, pBuffer, *pBytesReadOut, maxBytes);
    *pBytesReadOut += sizeof(NumCollisionShapes);

    READ(CollisionFlags, pBuffer, *pBytesReadOut, maxBytes);
    *pBytesReadOut += sizeof(CollisionFlags);

    return true;
}