#pragma once

#include <cstdint>

namespace sputter { namespace render {
    struct Glyph
    {
        static const Glyph kInvalidGlyph;

        bool IsValid() const;

        uint32_t Width;
        uint32_t Height;
        uint8_t* pGlyphPixels;
    };
}}