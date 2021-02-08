#include "vector2d.h"

namespace sputter { namespace math {
    Vector2D::Vector2D()
        : Vector2D(0.f, 0.f) {}

    Vector2D::Vector2D(float x, float y)
        : m_x(x), m_y(y) {}

    float Vector2D::GetX() const
    {
        return m_x;
    }

    float Vector2D::GetY() const
    {
        return m_y;
    }

    void Vector2D::Set(float x, float y)
    {
        m_x = x; m_y = y;
    }

    void Vector2D::Set(float v[2])
    {
        m_data[0] = v[0]; m_data[1] = v[1];
    }


    void Vector2D::SetX(float x)
    {
        m_x = x;
    }

    void Vector2D::SetY(float y)
    {
        m_y = y;
    }

    Vector2D Vector2D::operator+(const Vector2D& other) const
    {
        Vector2D returnValue = *this;
        returnValue.m_x += other.m_x;
        returnValue.m_y += other.m_y;

        return returnValue;
    }

    Vector2D& operator+=(const Vector2D& other)
    {
        this->m_x += other.m_x;
        this->m_y += other.m_y;

        return *this;
    }

    Vector2D Vector2D::operator*(float scalar) const
    {
        Vector2D returnValue = *this;
        returnValue.m_x *= scalar;
        returnValue.m_y *= scalar;

        return returnValue;
    }

    // 'cause scalar multiplication takes that long commute
    Vector2D operator*(float scalar, const Vector2D& other)
    {
        return other * scalar;
    }
}}
