#pragma once

// Fixed-point 2-dimensional vector

#include "fixedpoint.h"
#include <glm/glm.hpp>

namespace sputter { namespace math {
    class FPVector2D
    {
    public:
        static FPVector2D ZERO;
        static FPVector2D LEFT;
        static FPVector2D RIGHT;

        FPVector2D();
        FPVector2D(int x, int y);
        FPVector2D(FixedPoint x, FixedPoint y);

        // Accessors
        FixedPoint GetX() const;
        FixedPoint GetY() const;

        // Mutators
        void Set(const FPVector2D& other);
        void Set(FixedPoint x, FixedPoint y);
        void Set(FixedPoint v[2]);

        void SetX(FixedPoint x);
        void SetY(FixedPoint y);

        // Scalar Arithmetic
        FPVector2D operator+(const FPVector2D& other) const;
        FPVector2D operator+=(const FPVector2D& other);

        FPVector2D operator*(FixedPoint scalar) const;
        FPVector2D operator/(FixedPoint scalar) const;

        // Unary vector operations, computations
        FPVector2D operator-() const;

        FixedPoint Length() const;
        FPVector2D Normalized() const;

        bool IsZero() const;

        // Conversions
        glm::vec2 ToVec2() const;

        // TODO: Everything else
    private:
        FixedPoint m_x;
        FixedPoint m_y;
    };

    FPVector2D operator*(FixedPoint scalar, const FPVector2D& other);
}}
