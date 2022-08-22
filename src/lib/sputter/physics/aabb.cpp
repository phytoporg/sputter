#include "aabb.h"

#include <cassert>

#include <fpm/math.hpp>

#include <sputter/math/fpconstants.h>

#include <sputter/system/system.h>

using namespace sputter::physics;
using namespace sputter::math;

AABB::AABB(const FPVector3D& lowerLeft, const FPVector3D& extents)
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

FPVector3D AABB::GetSeparation2D(const ICollisionShape* pOtherShape) const
{
#if DEBUG
    if (!Intersects(pOtherShape))
    {
        sputter::system::LogAndFail("Retrieiving seapration vector for non-intersecting shapes.");
    }
#endif

    if (pOtherShape->GetShapeType() == CollisionShapeType::AABB)
    {
        const AABB* pOtherAABB = static_cast<const AABB*>(pOtherShape);
        const AABB MinkowskiDistance = GetMinkowskiDistance(*pOtherAABB);

        // See https://blog.hamaluik.ca/posts/simple-aabb-collision-using-minkowski-difference/
        // for an explanation!
        return MinkowskiDistance.GetClosesetPointOnBounds(FPVector3D::ZERO);
    }
    else
    {
        sputter::system::LogAndFail("Unsupported collision shape type with AABB!");
    }

    // Satisfy the compiler
    return FPVector3D::ZERO;
}

FPVector3D AABB::GetLowerLeft() const
{
    return m_lowerLeft;
}

FPVector3D AABB::GetExtents() const
{
    return m_extents;
}

AABB AABB::GetMinkowskiDistance(const AABB& other) const
{
    const FPVector3D OtherTopRight = other.m_lowerLeft + other.m_extents;
    return AABB(
        m_lowerLeft - OtherTopRight,
        m_extents + other.m_extents
    );
}

FPVector3D AABB::GetClosesetPointOnBounds(const FPVector3D& point) const
{
    // Check the distance along each axis against the min and max extents, 
    // then project onto the bounds for the minimum.

    // X
    FixedPoint minDistance = fpm::abs(point.GetX() - m_lowerLeft.GetX());
    FPVector3D boundsPoint = FPVector3D(m_lowerLeft.GetX(), point.GetY(), point.GetZ());

    const FPVector3D TopRight = m_lowerLeft + m_extents;
    if (fpm::abs(point.GetX() - TopRight.GetX()) < minDistance)
    {
        minDistance = fpm::abs(point.GetX() - TopRight.GetX());
        boundsPoint = FPVector3D(TopRight.GetX(), point.GetY(), point.GetZ());
    }

    // Y
    if (fpm::abs(point.GetY() - m_lowerLeft.GetY()) < minDistance)
    {
        minDistance = fpm::abs(point.GetY() - m_lowerLeft.GetY());
        boundsPoint = FPVector3D(point.GetX(), m_lowerLeft.GetY(), point.GetZ());
    }

    if (fpm::abs(point.GetY() - TopRight.GetY()) < minDistance)
    {
        minDistance = fpm::abs(point.GetY() - TopRight.GetY());
        boundsPoint = FPVector3D(point.GetX(), TopRight.GetY(), point.GetZ());
    }

    // Z
    // Skip Z-axis checks, we're just doing 2D collision detection out here.

    /*
    if (fpm::abs(point.GetZ() - m_lowerLeft.GetZ()) < minDistance)
    {
        minDistance = fpm::abs(point.GetZ() - m_lowerLeft.GetZ());
        boundsPoint = FPVector3D(point.GetX(), point.GetY(), m_lowerLeft.GetZ());
    }

    if (fpm::abs(point.GetZ() - TopRight.GetZ()) < minDistance)
    {
        minDistance = fpm::abs(point.GetZ() - TopRight.GetZ());
        boundsPoint = FPVector3D(point.GetX(), point.GetY(), TopRight.GetZ());
    }
    */

    return boundsPoint;
}

void AABB::SetLowerLeft(const FPVector3D& lowerLeft)
{
    m_lowerLeft = lowerLeft;
}

void AABB::SetExtents(const FPVector3D& extents)
{
    m_extents = extents;
}