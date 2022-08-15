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
            const auto minX = (float)m_lowerLeft.GetX();
            const auto minY = (float)m_lowerLeft.GetY();
            const auto minZ = (float)m_lowerLeft.GetZ();

            const auto maxX = (float)(m_lowerLeft.GetX() + m_extents.GetX());
            const auto maxY = (float)(m_lowerLeft.GetY() + m_extents.GetY());
            const auto maxZ = (float)(m_lowerLeft.GetZ() + m_extents.GetZ());

            const auto oMinX = (float)(pOtherAABB->m_lowerLeft.GetX());
            const auto oMinY = (float)(pOtherAABB->m_lowerLeft.GetY());
            const auto oMinZ = (float)(pOtherAABB->m_lowerLeft.GetZ());

            const auto oMaxX = (float)(pOtherAABB->m_lowerLeft.GetX() + pOtherAABB->m_extents.GetX());
            const auto oMaxY = (float)(pOtherAABB->m_lowerLeft.GetY() + pOtherAABB->m_extents.GetY());
            const auto oMaxZ = (float)(pOtherAABB->m_lowerLeft.GetZ() + pOtherAABB->m_extents.GetZ());

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