#pragma once

#include <memory>
#include <string>
#include <vector>

#include "glyph.h"

namespace sputter { namespace render {
    class TrueTypeParser;

    class Font
    {
    public:
        Font(const std::string& name, TrueTypeParser* pParser);
        ~Font();

        const std::string& GetName() const;
        bool GetGlyph(char character, Glyph* pOut);
        bool GetGlyphMetrics(char character, GlyphMetrics* pOut);

    private:
        std::string        m_name;
        std::vector<Glyph> m_glyphCache;

        TrueTypeParser*    m_pParser = nullptr;
    };

    typedef std::shared_ptr<Font> FontPtr;
}}