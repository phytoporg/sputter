#include "font.h"

#include <cstring>

using namespace sputter::render;

bool Glyph::IsValid() const 
{
    return memcmp(this, &kInvalidGlyph, sizeof(*this)) == 0;
}

const Glyph Glyph::kInvalidGlyph = Glyph{
    static_cast<uint32_t>(-1),
    static_cast<uint32_t>(-1),
    nullptr
    };

Font::Font(const std::string& name, const std::vector<Glyph>& glyphTable) 
    : m_name(name), m_glyphTable(glyphTable)
{}

Font::~Font()
{
    // The font owns the glyphs. Free all of the memories !!
    for (Glyph& glyph : m_glyphTable)
    {
        delete[] glyph.pBitMatrix;
    }
}

const std::string& Font::GetName() const 
{
    return m_name;
}

bool Font::GetGlyph(char character, Glyph* pOut) const 
{
    const auto characterIndex = static_cast<size_t>(character);
    if (characterIndex >= m_glyphTable.size() || 
        !m_glyphTable[characterIndex].IsValid())
    {
        // Zero out the whole thing !
        *pOut = Glyph::kInvalidGlyph;
        return false;
    }

    *pOut = m_glyphTable[characterIndex];    
    return true;
}
