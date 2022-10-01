#include "fpvector2d.h"

#include "fpconstants.h"
#include <fpm/math.hpp>

using namespace sputter::math;

FPVector2D FPVector2D::ZERO(0, 0);
FPVector2D FPVector2D::LEFT(-1, 0);
FPVector2D FPVector2D::RIGHT(1, 0);
FPVector2D FPVector2D::DOWN(0, -1);

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

void FPVector2D::SetX(FixedPoint x)
{
    m_x = x;
}

void FPVector2D::SetY(FixedPoint y)
{
    m_y = y;
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

FPVector2D FPVector2D::operator-(const FPVector2D& other) const
{
    return FPVector2D(m_x - other.m_x, m_y - other.m_y);
}

FPVector2D FPVector2D::operator-=(const FPVector2D& other)
{
    m_x -= other.m_x;
    m_y -= other.m_y;

    return *this;
}

FPVector2D FPVector2D::operator*(FixedPoint scalar) const
{
    return FPVector2D(m_x * scalar, m_y * scalar);
}

FPVector2D FPVector2D::operator/(FixedPoint scalar) const
{
    return FPVector2D(m_x / scalar, m_y / scalar);
}

FPVector2D FPVector2D::operator-() const
{
    return FPVector2D(-m_x, -m_y);
}

FixedPoint FPVector2D::Length() const
{
    return fpm::sqrt(m_x * m_x + m_y * m_y);
}

FPVector2D FPVector2D::Normalized() const
{
    return *this / Length();
}

bool FPVector2D::IsZero() const
{
    return m_x == FPZero && m_y == FPZero;
}

glm::vec2 sputter::math::FPVector2D::ToVec2() const 
{
    return glm::vec2(float(m_x), float(m_y));
}
