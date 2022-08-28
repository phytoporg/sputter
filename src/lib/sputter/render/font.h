#pragma once

#include <memory>
#include <string>
#include <vector>

namespace sputter { namespace render {
    struct Glyph
    {
        static const Glyph kInvalidGlyph;
        bool IsValid() const;

        uint32_t Width;
        uint32_t Height;
        bool*    pBitMatrix;
    };

    class Font
    {
    public:
        Font(const std::string& name, const std::vector<Glyph>& glyphTable);
        ~Font();

        const std::string& GetName() const;
        bool GetGlyph(char character, Glyph* pOut) const;

    private:
        std::string        m_name;
        std::vector<Glyph> m_glyphTable;
    };

    typedef std::shared_ptr<Font> FontPtr;
}}