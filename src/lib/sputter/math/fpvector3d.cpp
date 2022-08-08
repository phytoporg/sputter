#include "fpvector3d.h"

namespace sputter { namespace math {
    FPVector3D::FPVector3D()
        : m_x(0), m_y(0), m_z(0)
    {}

    FPVector3D::FPVector3D(FixedPoint x, FixedPoint y, FixedPoint z)
        : m_x(x), m_y(y), m_z(z)
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
}}
