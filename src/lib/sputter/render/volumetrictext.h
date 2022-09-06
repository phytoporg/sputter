#pragma once

#include <cstdint>
#include <string>
#include <memory>

#include <glm/glm.hpp>

namespace sputter {
    namespace assets {
        class AssetStorage;
    }
}

namespace sputter { namespace render {
    class ShaderStorage;
    class FontStorage;

    class VolumetricTextRenderer
    {
    public:
        VolumetricTextRenderer(
            assets::AssetStorage* pAssetStorage,
            ShaderStorage* pShaderStorage,
            FontStorage* pFontStorage
            );

        // TODO: Shouldn't have to provide this?
        void SetMatrices(const glm::mat4& projMatrix, const glm::mat4& viewMatrix);

        void DrawText(int32_t x, int32_t y, uint32_t size, const char* pText);

    private:
        struct PImpl;
        std::unique_ptr<PImpl> m_spPimpl;
    };
}}