#include "aabb.h"

#include <sputter/system/system.h>

using namespace sputter::physics;
using namespace sputter::math;

AABB::AABB(const sputter::math::FPVector3D& upperLeft, const sputter::math::FPVector3D& extents)
    : m_upperLeft(upperLeft), m_extents(extents)
{}

CollisionShapeType AABB::GetShapeType() const 
{
    return CollisionShapeType::AABB;
}

bool AABB::Intersects(ICollisionShape* const pOtherShape) const
{
    switch (pOtherShape->GetShapeType())
    {
    case CollisionShapeType::AABB:
        {
            AABB* pOtherAABB = static_cast<AABB*>(pOtherShape);
            const FixedPoint minX = m_upperLeft.GetX();
            const FixedPoint minY = m_upperLeft.GetY();
            const FixedPoint minZ = m_upperLeft.GetZ();
            const FixedPoint maxX = m_upperLeft.GetX() + m_extents.GetX();
            const FixedPoint maxY = m_upperLeft.GetY() + m_extents.GetY();
            const FixedPoint maxZ = m_upperLeft.GetZ() + m_extents.GetZ();

            const FixedPoint oMinX = pOtherAABB->m_upperLeft.GetX();
            const FixedPoint oMinY = pOtherAABB->m_upperLeft.GetY();
            const FixedPoint oMinZ = pOtherAABB->m_upperLeft.GetZ();
            const FixedPoint oMaxX = pOtherAABB->m_upperLeft.GetX() + m_extents.GetX();
            const FixedPoint oMaxY = pOtherAABB->m_upperLeft.GetY() + m_extents.GetY();
            const FixedPoint oMaxZ = pOtherAABB->m_upperLeft.GetZ() + m_extents.GetZ();

            return (minX <= oMaxX && maxX >= oMinX) &&
                   (minY <= oMaxY && maxY >= oMinY) &&
                   (minZ <= oMaxZ && maxZ >= oMinZ);
        }
        break;
    
    default:
        sputter::system::LogAndFail("Unsupported intersection test!");
        return false;
    }
}