#include "fptransform3d.h"

#include <glm/gtx/euler_angles.hpp>

#include <cstdio>

using namespace sputter::math;

FPTransform3D::FPTransform3D() 
    : m_translation(0, 0, 0), m_scale(1, 1, 1), m_rotation(0, 0, 0)
{}

FPTransform3D FPTransform3D::Identity() 
{
    static const FPTransform3D identity;
    return identity;
}

void FPTransform3D::SetTranslation(const FPVector3D& translation) 
{
    m_translation = translation;
}

void FPTransform3D::SetScale(const FPVector3D& scale) 
{
    m_scale = scale;
}

void FPTransform3D::SetRotation(const FPVector3D& rotation) 
{
    m_rotation = rotation;
}

FPVector3D FPTransform3D::GetTranslation() const 
{
    return m_translation;
}

FPVector3D FPTransform3D::GetScale() const 
{
    return m_scale;
}

FPVector3D FPTransform3D::GetRotation() const 
{
    return m_rotation;
}

glm::mat4 FPTransform3D::ToMat4() const 
{
    const glm::vec3 rotation    = m_rotation.ToVec3();
    const glm::vec3 scale       = m_scale.ToVec3();
    const glm::vec3 translation = m_translation.ToVec3();

    glm::mat4 returnValue = glm::eulerAngleXYZ(rotation.x, rotation.y, rotation.z); 
    returnValue[0][0] *= scale.x;
    returnValue[1][1] *= scale.y;
    returnValue[2][2] *= scale.z;
    returnValue[3] = glm::vec4(translation, 1.0f);

    return returnValue;
}

void ToString(const FPTransform3D& transform, char *pBuffer)
{
    char translationBuffer[128];
    ToString(transform.GetTranslation(), translationBuffer);
    char scaleBuffer[128];
    ToString(transform.GetScale(), scaleBuffer);
    char rotationBuffer[128];
    ToString(transform.GetRotation(), rotationBuffer);
    sprintf(
        pBuffer,
        "translation: %s | scale: %s | rotation: %s",
        translationBuffer, scaleBuffer, rotationBuffer);
}
