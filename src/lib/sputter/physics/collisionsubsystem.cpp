#include "collisionsubsystem.h"

#include <sputter/system/system.h>
#include <sputter/core/check.h>

using namespace sputter::core;
using namespace sputter::physics;

CollisionResult::CollisionResult(
    const Collision* pA,
    const Collision* pB,
    const AABB* pShapeA,
    const AABB* pShapeB)
    : pCollisionA(pA), pCollisionB(pB), pCollisionShapeA(pShapeA), pCollisionShapeB(pShapeB)
{}

CollisionSubsystem::CollisionSubsystem(const CollisionSubsystemSettings& settings)
{}

void CollisionSubsystem::PostTick(math::FixedPoint dt) 
{
    NumCollisionsThisFrame = 0;
    for (size_t i = 0; i < m_collisionCount; i++)
    {
        for (size_t j = i + 1; j < m_collisionCount; ++j)
        {
            Collision& A = m_collisions[i];
            Collision& B = m_collisions[j];

            // No self-collisions for now
            if (A.ObjectHandle == B.ObjectHandle)
            {
                continue;
            }

            CollisionResult result;
            if ((A.CollisionFlags & B.CollisionFlags) && A.TestIntersection(B, &result))
            {
                RELEASE_CHECK(NumCollisionsThisFrame < kMaxCollisionResults, "Exceeded max collision results this frame");
                CollisionsThisFrame[NumCollisionsThisFrame++] = result;
            }
        }
    }
}

Collision* CollisionSubsystem::CreateComponent(const Collision::InitializationParameters& params) 
{
    RELEASE_CHECK(m_collisionCount < kMaxCollisions, "Exceeded maximum collisions count");

    m_collisions[m_collisionCount] = Collision();
    return &m_collisions[m_collisionCount++];
}

void CollisionSubsystem::ReleaseComponent(Collision* pComponent) 
{
    // TODO
}

bool CollisionSubsystem::Serialize(void* pBuffer, size_t* pBytesWrittenOut, size_t maxBytes)
{
    WRITE(m_collisionCount, pBuffer, *pBytesWrittenOut, maxBytes);
    *pBytesWrittenOut += sizeof(m_collisionCount);

    WRITE_ARRAY(m_collisions, pBuffer, *pBytesWrittenOut, maxBytes);
    *pBytesWrittenOut += sizeof(m_collisions);

    return true;
}

bool CollisionSubsystem::Deserialize(void* pBuffer, size_t* pBytesReadOut, size_t maxBytes)
{
    READ(m_collisionCount, pBuffer, *pBytesReadOut, maxBytes);
    *pBytesReadOut += sizeof(m_collisionCount);

    RELEASE_CHECK(m_collisionCount < kMaxCollisions, "Invalid collision count read while deserializing");

    READ_ARRAY(m_collisions, pBuffer, *pBytesReadOut, maxBytes);
    *pBytesReadOut += sizeof(m_collisions);

    return true;
}

ComponentHandle CollisionSubsystem::GetComponentHandle(Collision* pCollision) const
{
    for (size_t i = 0; i < m_collisionCount; ++i)
    {
        if (pCollision == &m_collisions[i])
        {
            return CreateComponentHandle<Collision>(i);
        }
    }

    return kInvalidComponentHandle;
}

Collision* CollisionSubsystem::GetComponentFromHandle(ComponentHandle handle)
{
    RELEASE_CHECK(handle != kInvalidComponentHandle, "Invalid collision handle");
    const uint16_t Index = GetComponentIndexFromHandle(handle);
    return &m_collisions[Index];
}

bool CollisionSubsystem::GetCollisionResultsForThisFrame(
    Collision* pCollision,
    containers::ArrayVector<CollisionResult, kMaxCollisionResults>* pResultsOut)
{
    RELEASE_CHECK(pResultsOut, "pResultsOut is unexpectedly null");

    pResultsOut->Clear();
    for (size_t i = 0; i < NumCollisionsThisFrame; ++i)
    {
        const CollisionResult& CollisionResult = CollisionsThisFrame[i];
        if (CollisionResult.pCollisionA == pCollision || CollisionResult.pCollisionB == pCollision)
        {
            pResultsOut->Add(CollisionResult);
        }
    }

    return true;
}

bool CollisionSubsystem::GetCollisionResultsForThisFrame(
    core::ComponentHandle collisionHandle,
    containers::ArrayVector<CollisionResult, kMaxCollisionResults>* pResultsOut)
{
    Collision* pCollision = GetComponentFromHandle(collisionHandle);
    if (!pCollision)
    {
        return false;
    }

    return GetCollisionResultsForThisFrame(pCollision, pResultsOut);
}