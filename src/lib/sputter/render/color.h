#pragma once

#include <cstdint>

#include <glm/glm.hpp>

namespace sputter { namespace render { 
    struct Color
    {
        uint8_t Red;
        uint8_t Green;
        uint8_t Blue;

        glm::vec3 ToVec3() const;
    };
}}