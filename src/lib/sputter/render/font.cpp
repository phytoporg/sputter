#include "font.h"
#include "truetypeparser.h"

#include <sputter/system/system.h>

#include <cstring>

using namespace sputter::render;

Font::Font(const std::string& name, TrueTypeParser* pParser) 
    : m_name(name), m_pParser(pParser), m_glyphCache(256, Glyph::kInvalidGlyph)
{}

Font::~Font()
{
    // The font owns the glyphs. Free all of the memories !!
    for (Glyph& glyph : m_glyphCache)
    {
        delete[] glyph.pGlyphPixels;
    }

    delete m_pParser;
}

const std::string& Font::GetName() const 
{
    return m_name;
}

bool Font::GetGlyph(char character, Glyph* pOut)
{
    const auto characterIndex = static_cast<size_t>(character);
    if (characterIndex >= m_glyphCache.size() || 
        !m_glyphCache[characterIndex].IsValid())
    {
        // Cache miss, generate the glyph.
        const Glyph glyph = m_pParser->GetCharacterGlyph(character);
        if (glyph.IsValid())
        {
            if (characterIndex > m_glyphCache.size())
            {
                LOG(WARNING) << "Character index is out of bounds for the glyph cache.";
                m_glyphCache.resize(characterIndex);
            }

            m_glyphCache[characterIndex] = glyph;
            *pOut = glyph;

            return true;
        }

        // Could not get a valid glyph
        *pOut = Glyph::kInvalidGlyph;
        return false;
    }

    *pOut = m_glyphCache[characterIndex];    
    return true;
}

bool Font::GetGlyphMetrics(char character, GlyphMetrics* pOut)
{
    *pOut = m_pParser->GetCharacterGlyphMetrics(character);
    return true;
}
