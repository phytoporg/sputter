#pragma once

#include "color.h"

#include <cstdint>

#include <glm/glm.hpp>

// Forward declarations
namespace sputter {
    namespace render {
        class Shader;
    }
}

namespace sputter { namespace render { namespace shapes {
    // Gotta call this before Draw* 
    bool InitializeLineRenderer(Shader* pShader, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

    void DrawRect(int32_t x, int32_t y, int32_t width, int32_t height, int32_t borderSize, const Color& borderColor);
}}}