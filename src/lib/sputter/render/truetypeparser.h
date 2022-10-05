#pragma once

#include "truetypetables.h"
#include "glyph.h"

namespace sputter { namespace assets {
    struct BinaryData;
}}

namespace sputter { namespace render {
    class TrueTypeParser
    {
    public:
        TrueTypeParser(const assets::BinaryData& dataToParse);
        bool IsGood();

        // TODO: things to retrieve after parsing
        Glyph GetCharacterGlyph(char c);
        GlyphMetrics GetCharacterGlyphMetrics(char c);

    private:
        const GLYPH_Header* FindGlyphHeader(char c, uint16_t* pGlyphIndexOut = nullptr);

        bool                      m_isGood;

        CMAP_SegmentMapPointers   m_CmapSegmentMapPointers;
        const CMAP_SegmentMapHeader* m_pCmapSegmentMap = nullptr;
        const EBLC_Header*  m_pEblcHeader = nullptr;
        const EBDT_Header*  m_pEbdtHeader = nullptr;
        const EBSC_Header*  m_pEbscHeader = nullptr;
        const CMAP_Header*  m_pCmapHeader = nullptr;
        const GLYPH_Header* m_pFirstGlyphHeader = nullptr;
        const HEAD_Header*  m_pHeadHeader = nullptr;
        const HHEA_Header*  m_pHheaHeader = nullptr;
        const VHEA_Header*  m_pVheaHeader = nullptr;
        const HMTX*         m_pHmtx       = nullptr;
        const VMTX*         m_pVmtx       = nullptr;
        const LOCA_Header*  m_pLocaHeader = nullptr;
        const MAXP_Header*  m_pMaxpHeader = nullptr;
    };
}}
