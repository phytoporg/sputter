#include "fptransform3d.h"

using namespace sputter::math;

FPTransform3D::FPTransform3D() 
    : m_location(0, 0, 0), m_scale(1, 1, 1), m_rotation(0, 0, 0)
{}

FPTransform3D FPTransform3D::Identity() 
{
    static const FPTransform3D identity;
    return identity;
}

void FPTransform3D::SetLocation(const FPVector3D& location) 
{
    m_location = location;
}

void FPTransform3D::SetScale(const FPVector3D& scale) 
{
    m_scale = scale;
}

glm::mat4 FPTransform3D::ToMat4() const 
{
    glm::mat4 returnValue(1.0);

    const glm::vec3 scale = m_scale.ToVec3();
    returnValue[0][0] = scale[0];
    returnValue[1][1] = scale[1];
    returnValue[2][2] = scale[2];

    const glm::vec3 location = m_location.ToVec3();
    returnValue[3] = glm::vec4(location, 1.0f);

    // TODO: Rotation

    return returnValue;
}

FPVector3D FPTransform3D::GetLocation() const 
{
    return m_location;
}
