#pragma once

#include <cstdint>

#include <glm/glm.hpp>

namespace sputter { namespace render { 
    struct Color
    {
        uint8_t R;
        uint8_t G;
        uint8_t B;

        glm::vec3 ToVec3() const;

        static Color RED;
        static Color DARKRED;
        static Color BLUE;
        static Color GREEN;
        static Color WHITE;
        static Color BLACK;
        static Color GRAY;
    };
}}
