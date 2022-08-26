#pragma once

#include <cstdint>
#include <string>
#include <memory>

namespace sputter {
    namespace assets {
        class AssetStorage;
    }
}

namespace sputter { namespace render {
    class ShaderStorage;

    struct Glyph
    {
        uint32_t Width;
        uint32_t Height;
        bool*    pBitMatrix;
    };

    class VolumetricTextRenderer
    {
    public:
        VolumetricTextRenderer(assets::AssetStorage* pAssetStorage, ShaderStorage* pShaderStorage);

        void DrawText(uint32_t x, uint32_t y, uint32_t size, const char* pText);

    private:
        struct PImpl;
        std::unique_ptr<PImpl> m_spPimpl;
    };
}}