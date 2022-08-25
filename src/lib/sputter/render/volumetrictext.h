#pragma once

#include "shader.h"

#include <cstdint>
#include <memory>

namespace sputter { namespace render {
    struct Glyph
    {
        uint32_t Width;
        uint32_t Height;
        bool*    pBitMatrix;
    };

    class VolumetricTextRenderer
    {
    public:
        VolumetricTextRenderer(ShaderPtr spShader);

        void DrawText(uint32_t x, uint32_t y, uint32_t size, const char* pText);

    private:
        struct PImpl;
        std::unique_ptr<PImpl> m_spPimpl;
    };
}}