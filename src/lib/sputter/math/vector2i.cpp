#include "vector2i.h"

using namespace sputter::math;

Vector2i Vector2i::Zero = Vector2i(0, 0);

Vector2i::Vector2i()
    : m_x(0), m_y(0)
{}

Vector2i::Vector2i(int x, int y)
    : m_x(x), m_y(y)
{}

int Vector2i::GetX() const
{
    return m_x;
}

int Vector2i::GetY() const
{
    return m_y;
}

void Vector2i::Set(const Vector2i& other)
{
    m_x = other.m_x;
    m_y = other.m_y;
}

void Vector2i::Set(int x, int y)
{
    m_x = x; m_y = y;
}

void Vector2i::Set(int v[2])
{
    m_x = v[0];
    m_y = v[1];
}

void Vector2i::SetX(int x)
{
    m_x = x;
}

void Vector2i::SetY(int y)
{
    m_y = y;
}

Vector2i Vector2i::operator+(const Vector2i& other) const
{
    return Vector2i(m_x + other.m_x, m_y + other.m_y);
}

Vector2i& Vector2i::operator+=(const Vector2i& other)
{
    m_x += other.m_x;
    m_y += other.m_y;

    return *this;
}

Vector2i Vector2i::operator*(int scalar) const
{
    return Vector2i(m_x * scalar, m_y * scalar);
}