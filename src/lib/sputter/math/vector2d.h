#pragma once

namespace sputter { namespace math {
    class Vector2D
    {
    public:
        Vector2D();
        Vector2D(float x, float y);

        // Accessors
        float GetX() const;
        float GetY() const;

        // Mutators
        void Set(float x, float y);
        void Set(float v[2]);

        void SetX(float x);
        void SetY(float y);

        // Addition
        Vector2D operator+(const Vector2D& other) const;
        Vector2D& operator+=(const Vector2D& other);

        // Scalar multiplication
        Vector2D operator*(float scalar) const;
        
        // TODO: everything else

    private:
        union
        {
            float m_data[2];

            struct 
            {
                float m_x;
                float m_y;
            };
        };
    };

    Vector2D operator*(float scalar, const Vector2D& other);
}}
