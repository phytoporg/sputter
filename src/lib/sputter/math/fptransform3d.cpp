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

FPMatrix4D FPTransform3D::ToFPMat4D() const
{
    // TODO
}

glm::mat4 FPTransform3D::ToMat4() const;
{
    // TODO
}
