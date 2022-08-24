#pragma once

#include <cstdint>

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
        VolumetricTextRenderer();

    private:
        Glyph m_glyphLookup[256];
    };
}}