#pragma once

#include <cstdint>

namespace sputter { namespace math {
    class Vector2i
    {
    public:
        Vector2i();
        Vector2i(int x, int y);

        // Accessors
        int GetX() const;
        int GetY() const;

        // Mutators
        void Set(int x, int y);
        void Set(int v[2]);
        void Set(const Vector2i& other);

        void SetX(int x);
        void SetY(int y);

        // Addition
        Vector2i operator+(const Vector2i& other) const;
        Vector2i& operator+=(const Vector2i& other);

        // Scalar multiplication
        Vector2i operator*(int scalar) const;
        
        // TODO: everything else
        static Vector2i Zero;

    private:
        union
        {
            int m_data[2];

            struct 
            {
                int m_x;
                int m_y;
            };
        };
    };
}}
