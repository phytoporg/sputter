#pragma once

#include <sputter/math/fpvector3d.h>

#include <glm/glm.hpp>

namespace sputter { namespace math {
    class FPTransform3D
    {
    public:
        // Identity by default
        FPTransform3D();

        static FPTransform3D Identity();

        void SetLocation(const FPVector3D& location);
        void SetScale(const FPVector3D& scale);

        FPVector3D GetLocation() const;

        // TODO:
        // void SetRotation(const FPVector3D& rotation);
        //
        // FPMatrix4D ToFPMat4D() const;
        glm::mat4  ToMat4() const;

    private:
        // x, y, z
        FPVector3D m_location;

        // Scale along x, y, z
        FPVector3D m_scale;
        
        // Roll, pitch, yaw
        FPVector3D m_rotation;
    };
} }