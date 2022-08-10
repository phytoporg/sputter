#include "fpvector3d.h"
#include <fpm/math.hpp>

using namespace sputter::math;

const FPVector3D FPVector3D::ZERO = FPVector3D(0, 0, 0);

FPVector3D::FPVector3D()
    : m_x(0), m_y(0), m_z(0)
{}

FPVector3D::FPVector3D(FixedPoint x, FixedPoint y, FixedPoint z)
    : m_x(x), m_y(y), m_z(z)
{}

FPVector3D::FPVector3D(int x, int y, int z)
    : m_x(FixedPoint(x)), m_y(FixedPoint(y)), m_z(FixedPoint(z))
{}

FixedPoint FPVector3D::GetX() const
{
    return m_x;
}

FixedPoint FPVector3D::GetY() const
{
    return m_y;
}

FixedPoint FPVector3D::GetZ() const
{
    return m_z;
}

void FPVector3D::Set(const FPVector3D& other)
{
    m_x = other.m_x;
    m_y = other.m_y;
    m_z = other.m_z;
}

void FPVector3D::Set(FixedPoint x, FixedPoint y, FixedPoint z)
{
    m_x = x; m_y = y; m_z = z;
}

void FPVector3D::Set(FixedPoint v[2])
{
    m_x = v[0];
    m_y = v[1];
    m_z = v[2];
}

FPVector3D FPVector3D::operator+(const FPVector3D& other) const
{
    return FPVector3D(m_x + other.m_x, m_y + other.m_y, m_z + other.m_z);
}

FPVector3D FPVector3D::operator+=(const FPVector3D& other)
{
    m_x += other.m_x;
    m_y += other.m_y;
    m_z += other.m_z;

    return *this;
}

FPVector3D FPVector3D::operator*(FixedPoint scalar) const
{
    return FPVector3D(m_x * scalar, m_y * scalar, m_z * scalar);
}

FPVector3D operator*(FixedPoint scalar, const FPVector3D& other)
{
    return other * scalar;
}

FPVector3D FPVector3D::operator/(FixedPoint scalar) const 
{
    return FPVector3D(m_x / scalar, m_y / scalar, m_z / scalar);
}

FixedPoint FPVector3D::Length() const 
{
    return fpm::sqrt(m_x * m_x + m_y * m_y + m_z * m_z);
}

FPVector3D FPVector3D::Normalized() const 
{
    return *this / Length();
}

glm::vec3 FPVector3D::ToVec3() const 
{
    return glm::vec3(float(m_x), float(m_y), float(m_z));
}