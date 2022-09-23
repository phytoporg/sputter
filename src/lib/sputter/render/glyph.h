#pragma once

#include <cstdint>

namespace sputter { namespace render {
    struct GlyphMetrics
    {
        uint32_t BearingX = 0;
        uint32_t Width    = 0;
        uint32_t Height   = 0;
    };

    struct Glyph
    {
        static const Glyph kInvalidGlyph;

        bool IsValid() const;

        GlyphMetrics Metrics;
        uint8_t*     pGlyphPixels = nullptr;
    };
}}