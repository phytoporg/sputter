#include "collisionsubsystem.h"

#include "collisionshape.h"

using namespace sputter::physics;

CollisionSubsystem::CollisionSubsystem(const CollisionSubsystemSettings& settings)
{
    // TODO: Fixed-size allocation :P
    m_collisions.reserve(100);
}

void CollisionSubsystem::Tick(math::FixedPoint dt) 
{
    m_collisionResults.clear();
    for (size_t i = 0; i < m_collisions.size(); i++)
    {
        for (size_t j = i + 1; j < m_collisions.size(); ++j)
        {
            const Collision& A = m_collisions[i];
            const Collision& B = m_collisions[j];

            // No self-collisions for now
            if (A.pObject == B.pObject)
            {
                continue;
            }

            if ((A.CollisionFlags & B.CollisionFlags) && A.Intersects(B))
            {
                CollisionResult result;
                result.pCollisionA = &A;
                result.pCollisionB = &B;

                m_collisionResults.push_back(result);
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

void CollisionSubsystem::ProcessCollisionResults()
{
    for (const CollisionResult& collisionResult : m_collisionResults)
    {
        // TODO: call HandleCollision() or some such? Messaging system? 
    }
}