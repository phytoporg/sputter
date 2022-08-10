#pragma once

#include <sputter/math/fpvector3d.h>
#include <sputter/math/fpmatrix4d.h>

#include <glm/glm.hpp>

namespace sputter { namespace render {
    class Camera
    {
    public:
        Camera();
        Camera(
            const sputter::math::FPVector3D& translation,
            const sputter::math::FPVector3D& forwardDirection,
            const sputter::math::FPVector3D& upDirection
        );

        sputter::math::FPVector3D GetTranslation() const;
        void SetTranslation(const sputter::math::FPVector3D& translation);

        void Translate(const sputter::math::FPVector3D& translationDelta);

        glm::mat4 ViewMatrix4d() const;

    private:
        sputter::math::FPVector3D m_translation;
        sputter::math::FPVector3D m_forwardDirection;
        sputter::math::FPVector3D m_upDirection;

        static const sputter::math::FPVector3D kDefaultTranslation;
        static const sputter::math::FPVector3D kDefaultForwardDirection;
        static const sputter::math::FPVector3D kDefaultUpDirection;
    };
}}
