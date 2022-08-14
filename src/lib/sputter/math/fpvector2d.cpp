#include "fpvector2d.h"

using namespace sputter::math;

FPVector2D::FPVector2D()
    : m_x(0), m_y(0)
{}

FPVector2D::FPVector2D(int x, int y)
    : m_x(x), m_y(y)
{}

FPVector2D::FPVector2D(FixedPoint x, FixedPoint y)
    : m_x(x), m_y(y)
{}

FixedPoint FPVector2D::GetX() const
{
    return m_x;
}

FixedPoint FPVector2D::GetY() const
{
    return m_y;
}

void FPVector2D::Set(const FPVector2D& other)
{
    m_x = other.m_x;
    m_y = other.m_y;
}

void FPVector2D::Set(FixedPoint x, FixedPoint y)
{
    m_x = x; m_y = y;
}

void FPVector2D::Set(FixedPoint v[2])
{
    m_x = v[0];
    m_y = v[1];
}

FPVector2D FPVector2D::operator+(const FPVector2D& other) const
{
    return FPVector2D(m_x + other.m_x, m_y + other.m_y);
}

FPVector2D FPVector2D::operator+=(const FPVector2D& other)
{
    m_x += other.m_x;
    m_y += other.m_y;

    return *this;
}

FPVector2D FPVector2D::operator*(FixedPoint scalar) const
{
    return FPVector2D(m_x * scalar, m_y * scalar);
}

FPVector2D operator*(FixedPoint scalar, const FPVector2D& other)
{
    return other * scalar;
}

glm::vec2 sputter::math::FPVector2D::ToVec2() const 
{
    return glm::vec2(float(m_x), float(m_y));
}
