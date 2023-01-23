#include "collisionsubsystem.h"
#include "collisionshape.h"

#include <sputter/system/system.h>
#include <sputter/core/check.h>

using namespace sputter::core;
using namespace sputter::physics;

CollisionResult::CollisionResult(
    const Collision* pA,
    const Collision* pB,
    const ICollisionShape* pShapeA,
    const ICollisionShape* pShapeB)
    : pCollisionA(pA), pCollisionB(pB), pCollisionShapeA(pShapeA), pCollisionShapeB(pShapeB)
{}

CollisionSubsystem::CollisionSubsystem(const CollisionSubsystemSettings& settings)
{}

void CollisionSubsystem::PostTick(math::FixedPoint dt) 
{
    for (size_t i = 0; i < m_collisionCount; i++)
    {
        m_collisions[i].NumCollisionsThisFrame = 0;
    }

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
                // TODO: collisions shouldn't necessarily be bidirectional. This is
                // wasteful in cases for instances where A has to take no action
                // in response to this collision, but B does.
                A.CollisionsThisFrame[A.NumCollisionsThisFrame++] = result;
                B.CollisionsThisFrame[B.NumCollisionsThisFrame++] = result;

               result.pCollisionShapeA->GetSeparation2D(result.pCollisionShapeB);
            }
        }
    }
}

Collision* CollisionSubsystem::CreateComponent(const Collision::InitializationParameters& params) 
{
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

    for (uint32_t i = 0; i < m_collisionCount; ++i)
    {
        if (!m_collisions[i].Serialize(pBuffer, pBytesWrittenOut, maxBytes))
        { 
            return false;
        }
    }

    return true;
}

bool CollisionSubsystem::Deserialize(void* pBuffer, size_t* pBytesReadOut, size_t maxBytes)
{
    READ(m_collisionCount, pBuffer, *pBytesReadOut, maxBytes);
    *pBytesReadOut += sizeof(m_collisionCount);

    RELEASE_CHECK(m_collisionCount < kMaxCollisions, "Invalid collision count read while deserializing");

    for (uint32_t i = 0; i < m_collisionCount; ++i)
    {
        if (!m_collisions[i].Deserialize(pBuffer, pBytesReadOut, maxBytes))
        {
            return false;
        }
    }

    return true;
}

ComponentHandle CollisionSubsystem::GetComponentHandle(Collision* pCollision) const
{
    for (uint16_t i = 0; i < m_collisionCount; ++i)
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
