#pragma once

#include "collisionshape.h"
#include <sputter/math/fpvector3d.h>

namespace sputter { namespace physics {
    class AABB : public ICollisionShape
    {
    public:
        AABB(const sputter::math::FPVector3D& upperLeft, const sputter::math::FPVector3D& extents);

        virtual CollisionShapeType GetShapeType() const override;
        virtual bool Intersects(ICollisionShape* const pOtherShape) const override;

        sputter::math::FPVector3D GetUpperLeft() const;
        sputter::math::FPVector3D GetExtents() const;

    private:
        sputter::math::FPVector3D m_upperLeft;
        sputter::math::FPVector3D m_extents;
    };
}}