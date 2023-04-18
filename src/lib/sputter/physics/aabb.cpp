#include "aabb.h"

#include <cassert>

#include <fpm/math.hpp>

#include <sputter/math/fpconstants.h>

#include <sputter/system/system.h>

using namespace sputter::physics;
using namespace sputter::math;

AABB::AABB()
    : m_lowerLeft(FPVector3D::ZERO), m_extents(FPVector3D::ZERO)
{}

AABB::AABB(const FPVector3D& lowerLeft, const FPVector3D& extents)
    : m_lowerLeft(lowerLeft), m_extents(extents)
{}

bool AABB::Intersects(const AABB& other) const
{
    const FixedPoint minX = m_lowerLeft.GetX();
    const FixedPoint minY = m_lowerLeft.GetY();
    const FixedPoint minZ = m_lowerLeft.GetZ();

    const FixedPoint maxX = (m_lowerLeft.GetX() + m_extents.GetX());
    const FixedPoint maxY = (m_lowerLeft.GetY() + m_extents.GetY());
    const FixedPoint maxZ = (m_lowerLeft.GetZ() + m_extents.GetZ());

    const FixedPoint oMinX = (other.m_lowerLeft.GetX());
    const FixedPoint oMinY = (other.m_lowerLeft.GetY());
    const FixedPoint oMinZ = (other.m_lowerLeft.GetZ());

    const FixedPoint oMaxX = (other.m_lowerLeft.GetX() + other.m_extents.GetX());
    const FixedPoint oMaxY = (other.m_lowerLeft.GetY() + other.m_extents.GetY());
    const FixedPoint oMaxZ = (other.m_lowerLeft.GetZ() + other.m_extents.GetZ());

    return (minX <= oMaxX && maxX >= oMinX) &&
           (minY <= oMaxY && maxY >= oMinY) &&
           (minZ <= oMaxZ && maxZ >= oMinZ);
}

FPVector3D AABB::GetSeparation2D(const AABB& other) const
{
    const AABB MinkowskiDistance = GetMinkowskiDistance(other);

    // See https://blog.hamaluik.ca/posts/simple-aabb-collision-using-minkowski-difference/
    // for an explanation! Pretty elegant.
    return MinkowskiDistance.GetClosesetPointOnBounds(FPVector3D::ZERO);
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

void ToString(const sputter::physics::AABB aabb, char *pBuffer)
{
    char lowerLeftBuffer[128];
    ToString(aabb.GetLowerLeft(), lowerLeftBuffer);

    char extentsBuffer[128];
    ToString(aabb.GetExtents(), extentsBuffer);

    sprintf(
        pBuffer,
        "LowerLeft: %s | Extents: %s",
        lowerLeftBuffer, extentsBuffer);
}
