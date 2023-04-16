#pragma once

// Fixed-point 3-dimensional vector

#include "fixedpoint.h"
#include <glm/glm.hpp>

namespace sputter { namespace math {
    class FPVector3D
    {
    public:
        // Static constant values first
        static const FPVector3D ZERO;
        static const FPVector3D UP;

        FPVector3D();
        FPVector3D(FixedPoint x, FixedPoint y, FixedPoint z);
        FPVector3D(int x, int y, int z);

        // Accessors
        FixedPoint GetX() const;
        FixedPoint GetY() const;
        FixedPoint GetZ() const;

        // Mutators
        void Set(const FPVector3D& other);
        void Set(FixedPoint x, FixedPoint y, FixedPoint z);
        void Set(FixedPoint v[3]);

        void SetX(FixedPoint x);
        void SetY(FixedPoint y);
        void SetZ(FixedPoint z);

        // Addition
        FPVector3D operator+(const FPVector3D& other) const;
        FPVector3D operator+=(const FPVector3D& other);

        // Subtraction
        FPVector3D operator-(const FPVector3D& other) const;
        FPVector3D operator-=(const FPVector3D& other);

        // Scalar arithmetic
        FPVector3D operator*(FixedPoint scalar) const;
        FPVector3D operator/(FixedPoint scalar) const;

        // Unary vector operations, computations
        FPVector3D operator-() const;

        FixedPoint Length() const;
        FPVector3D Normalized() const;
        bool IsZero() const;

        // Other exciting vector operations
        FixedPoint DotProduct(const FPVector3D& other) const;

        // Conversions
        glm::vec3 ToVec3() const;

        // TODO: Everything else
    private:
        FPVector3D(float x, float y, float z) = delete;
        FPVector3D(double x, double y, double z) = delete;

        FixedPoint m_x;
        FixedPoint m_y;
        FixedPoint m_z;
    };

    FPVector3D operator*(FixedPoint scalar, const FPVector3D& other);
}}

void ToString(const sputter::math::FPVector3D& vector, char* pBuffer);
