#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>

using namespace sputter::render;
using namespace sputter::math;

const FPVector3D Camera::kDefaultTranslation = FPVector3D::ZERO;
const FPVector3D Camera::kDefaultForwardDirection = FPVector3D(0, 0, 1);
const FPVector3D Camera::kDefaultUpDirection = FPVector3D(0, 1, 0);

Camera::Camera()
    : Camera(kDefaultTranslation, kDefaultForwardDirection, kDefaultUpDirection)
{}

Camera::Camera(
    const FPVector3D& translation,
    const FPVector3D& forwardDirection,
    const FPVector3D& upDirection
    ) : m_translation(translation),
        m_forwardDirection(forwardDirection),
        m_upDirection(upDirection)
{}

FPVector3D Camera::GetTranslation() const 
{
    return m_translation;
}

void Camera::SetTranslation(const FPVector3D& translation)
{
    m_translation = translation;
}

void Camera::Translate(const FPVector3D& translationDelta) 
{
    m_translation += translationDelta;
}

glm::mat4 Camera::ViewMatrix4d() const 
{
    return glm::lookAt(
        m_translation.ToVec3(),
        m_forwardDirection.ToVec3(),
        m_upDirection.ToVec3());
}
