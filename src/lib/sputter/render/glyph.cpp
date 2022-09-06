#include "glyph.h"

#include <cstring>

using namespace sputter::render;

bool Glyph::IsValid() const 
{
    return memcmp(this, &kInvalidGlyph, sizeof(*this)) != 0;
}

const Glyph Glyph::kInvalidGlyph = Glyph{
    static_cast<uint32_t>(-1),
    static_cast<uint32_t>(-1),
    nullptr
    };