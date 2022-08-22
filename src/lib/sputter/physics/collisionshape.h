#pragma once

#include <sputter/math/fpvector3d.h>

namespace sputter { namespace physics {
    enum class CollisionShapeType
    {
        Invalid = 0,
        AABB,
        MaxCollisionShapeType
    };

    class ICollisionShape
    {
    public:
        virtual CollisionShapeType GetShapeType() const = 0;
        virtual bool Intersects(ICollisionShape* const pOtherShape) const = 0;
        virtual sputter::math::FPVector3D GetSeparation2D(const ICollisionShape* pOtherShape) const = 0;
    };
}}