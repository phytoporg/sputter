#pragma once

#include "color.h"

#include <cstdint>

#include <glm/glm.hpp>

// Forward declarations
namespace sputter {
    namespace assets {
        class AssetStorage;
    }

    namespace render {
        class ShaderStorage;
    }
}

namespace sputter { namespace render { namespace shapes {
    // Gotta call this before Draw* 
    bool InitializeLineRenderer(assets::AssetStorage* pAssetStorage, ShaderStorage* pShaderStorage, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
    void UninitializeLineRenderer();

    void DrawRect(int32_t x, int32_t y, int32_t width, int32_t height, int32_t borderSize, const Color& borderColor);
}}}