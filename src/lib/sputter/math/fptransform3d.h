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

        void SetTranslation(const FPVector3D& translation);
        void SetScale(const FPVector3D& scale);

        // Radians, yaw/pitch/roll
        void SetRotation(const FPVector3D& rotation);

        FPVector3D GetTranslation() const;
        FPVector3D GetScale() const;
        FPVector3D GetRotation() const;

        // TODO:
        // FPMatrix4D ToFPMat4D() const;

        glm::mat4  ToMat4() const;

    private:
        // x, y, z
        FPVector3D m_translation;

        // Scale along x, y, z
        FPVector3D m_scale;
        
        // Roll, pitch, yaw
        FPVector3D m_rotation;
    };
}}

void ToString(const sputter::math::FPTransform3D& transform, char* pBuffer);