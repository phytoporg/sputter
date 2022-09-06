#include "font.h"
#include "truetypeparser.h"

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

Font::Font(const std::string& name, TrueTypeParser* pParser) 
    : m_name(name), m_pParser(pParser)
{}

Font::~Font()
{
    // The font owns the glyphs. Free all of the memories !!
    for (Glyph& glyph : m_glyphCache)
    {
        delete[] glyph.pBitMatrix;
    }

    delete m_pParser;
}

const std::string& Font::GetName() const 
{
    return m_name;
}

bool Font::GetGlyph(char character, Glyph* pOut) const 
{
    // TODO: USE THE PARSER BEFORE HITTING THE CACHE

    const auto characterIndex = static_cast<size_t>(character);
    if (characterIndex >= m_glyphCache.size() || 
        !m_glyphCache[characterIndex].IsValid())
    {
        // Zero out the whole thing !
        *pOut = Glyph::kInvalidGlyph;
        return false;
    }

    *pOut = m_glyphCache[characterIndex];    
    return true;
}
