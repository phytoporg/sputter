#include "aabb.h"

#include <sputter/system/system.h>

//TEMP
#include <iostream>
//TEMP

using namespace sputter::physics;
using namespace sputter::math;

AABB::AABB(const sputter::math::FPVector3D& lowerLeft, const sputter::math::FPVector3D& extents)
    : m_lowerLeft(lowerLeft), m_extents(extents)
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
            const FixedPoint minX = m_lowerLeft.GetX();
            const FixedPoint minY = m_lowerLeft.GetY();
            const FixedPoint minZ = m_lowerLeft.GetZ();

            const FixedPoint maxX = (m_lowerLeft.GetX() + m_extents.GetX());
            const FixedPoint maxY = (m_lowerLeft.GetY() + m_extents.GetY());
            const FixedPoint maxZ = (m_lowerLeft.GetZ() + m_extents.GetZ());

            const FixedPoint oMinX = (pOtherAABB->m_lowerLeft.GetX());
            const FixedPoint oMinY = (pOtherAABB->m_lowerLeft.GetY());
            const FixedPoint oMinZ = (pOtherAABB->m_lowerLeft.GetZ());

            const FixedPoint oMaxX = (pOtherAABB->m_lowerLeft.GetX() + pOtherAABB->m_extents.GetX());
            const FixedPoint oMaxY = (pOtherAABB->m_lowerLeft.GetY() + pOtherAABB->m_extents.GetY());
            const FixedPoint oMaxZ = (pOtherAABB->m_lowerLeft.GetZ() + pOtherAABB->m_extents.GetZ());

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

FPVector3D AABB::GetLowerLeft() const
{
    return m_lowerLeft;
}

FPVector3D AABB::GetExtents() const
{
    return m_extents;
}

void AABB::SetLowerLeft(const sputter::math::FPVector3D& lowerLeft)
{
    m_lowerLeft = lowerLeft;
}

void AABB::SetExtents(const sputter::math::FPVector3D& extents)
{
    m_extents = extents;
}