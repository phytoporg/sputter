#pragma once

// Fixed-point 2-dimensional vector

#include "fixedpoint.h"

namespace sputter { namespace math {
    class FPVector2D
    {
    public:
        FPVector2D();
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

        // Addition
        FPVector2D operator+(const FPVector2D& other) const;
        FPVector2D operator+=(const FPVector2D& other);

        // Scalar multiplication
        FPVector2D operator*(FixedPoint scalar) const;

        // TODO: Everything else
    private:
        FixedPoint m_x;
        FixedPoint m_y;
    };

    FPVector2D operator*(FixedPoint scalar, const FPVector2D& other);
}}
