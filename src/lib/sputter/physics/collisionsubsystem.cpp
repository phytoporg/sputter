#include "collisionsubsystem.h"

#include "collisionshape.h"

#include <sputter/game/object.h>

using namespace sputter::physics;

CollisionResult::CollisionResult(
    const Collision* pA,
    const Collision* pB,
    const ICollisionShape* pShapeA,
    const ICollisionShape* pShapeB)
    : pCollisionA(pA), pCollisionB(pB), pCollisionShapeA(pShapeA), pCollisionShapeB(pShapeB)
{}

CollisionSubsystem::CollisionSubsystem(const CollisionSubsystemSettings& settings)
{
    // TODO: Fixed-size allocation :P
    m_collisions.reserve(100);
}

void CollisionSubsystem::PostTick(math::FixedPoint dt) 
{
    for (size_t i = 0; i < m_collisions.size(); i++)
    {
        m_collisions[i].CollisionsThisFrame.clear();
    }

    for (size_t i = 0; i < m_collisions.size(); i++)
    {
        for (size_t j = i + 1; j < m_collisions.size(); ++j)
        {
            Collision& A = m_collisions[i];
            Collision& B = m_collisions[j];

            // No self-collisions for now
            if (A.pObject == B.pObject)
            {
                continue;
            }

            CollisionResult result;
            if ((A.CollisionFlags & B.CollisionFlags) && A.TestIntersection(B, &result))
            {
                // TODO: collisions shouldn't necessarily be bidirectional. This is
                // wasteful in cases for instances where A has to take no action
                // in response to this collision, but B does.
                A.CollisionsThisFrame.push_back(result);
                B.CollisionsThisFrame.push_back(result);

               result.pCollisionShapeA->GetSeparation2D(result.pCollisionShapeB);
            }
        }
    }
}

Collision* CollisionSubsystem::CreateComponent(const Collision::InitializationParameters& params) 
{
    m_collisions.emplace_back();
    return &m_collisions.back();
}

void CollisionSubsystem::ReleaseComponent(Collision* pComponent) 
{
    // TODO
}