#pragma once

#include <sputter/math/fpvector3d.h>

namespace sputter { namespace physics {
    class AABB
    {
    public:
        AABB();
        AABB(const sputter::math::FPVector3D& lowerLeft, const sputter::math::FPVector3D& extents);

        bool Intersects(const AABB& other) const;
        sputter::math::FPVector3D GetSeparation2D(const AABB& other) const;

        sputter::math::FPVector3D GetLowerLeft() const;
        sputter::math::FPVector3D GetExtents() const;

        AABB GetMinkowskiDistance(const AABB& other) const;
        sputter::math::FPVector3D GetClosesetPointOnBounds(const sputter::math::FPVector3D& point) const;

        void SetLowerLeft(const sputter::math::FPVector3D& lowerLeft);
        void SetExtents(const sputter::math::FPVector3D& extents);

    private:
        sputter::math::FPVector3D m_lowerLeft;
        sputter::math::FPVector3D m_extents;
    };
}}