#include "truetypeparser.h"

#include <cstdint>

#include <sputter/assets/binarydata.h>
#include <sputter/system/system.h>

constexpr uint32_t FOURCC(const char* pString)
{
    return pString[0] | (pString[1] << 8) | (pString[2] << 16) | (pString[3] << 24);
}

namespace {
    // Yoinked straight from the spec
    uint32_t CalcTableChecksum(uint32_t *pTable, uint32_t numberOfBytesInTable)
    {
        uint32_t sum = 0;
        uint32_t nLongs = (numberOfBytesInTable + 3) / 4;
        while (nLongs-- > 0)
        {
            sum += *pTable++;
        }

        return sum;
    }

    // Wanted to get away from templates, but we're committed. BIG COMPILE TIMES LETS GOOOO
    template<typename T>
    const T* ReadNext(const char** pDataStream, size_t* pBytesRemaining)
    {
        const size_t Size = sizeof(T);
        if (*pBytesRemaining < Size)
        {
            return nullptr;
        }

        const T* pReturnValue = reinterpret_cast<const T*>(*pDataStream);
        *pBytesRemaining -= Size;
        *pDataStream += Size;

        return pReturnValue;
    }

    uint32_t SwapEndianness(uint32_t value)
    {
        return
            (value << 24) |
            (value & 0xFF00) << 8 |
            ((value >> 8)  & 0xFF00) |
            ((value >> 24) & 0xFF);
    }

    uint16_t SwapEndianness(uint16_t value)
    {
        return
            (value << 8) |
            ((value >> 8) & 0xFF);
    }
}

// See spec @ https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6.html
#pragma pack(push)
struct OffsetSubtable
{
    uint32_t ScalerType;
    uint16_t NumTables;
    uint16_t SearchRange;
    uint16_t EntrySelector;
    uint16_t RangeShift;
};

struct TableDirectory
{
    uint32_t Tag;
    uint32_t Checksum;
    uint32_t Offset;
    uint32_t Length;
};

// EBDT - Bitmap data table structures
struct EBDT_Header
{
    uint16_t MajorVersion;
    uint16_t MinorVersion;
};

struct EBDT_SmallGlyphMetrics
{
   uint8_t 	Height;
   uint8_t 	Width;
   int8_t 	BearingX;
   int8_t 	BearingY;
   uint8_t 	Advance;
};

struct EBDT_BitmapData
{
    uint8_t* pData;
};

// EBLC - Bitmap location data table structures
struct EBLC_SbitLineMetrics
{
    int8_t    Ascender;
    int8_t    Descender;
    uint8_t   WidthMax;
    int8_t    CaretSlopeNumerator;
    int8_t    CaretSlopeDenominator;
    int8_t    CaretOffset;
    int8_t    MinOriginSB;
    int8_t    MinAdvanceSB;
    int8_t    MaxBeforeBL;
    int8_t    MinAfterBL;
    int8_t    Pad1;
    int8_t    Pad2;
};

struct EBLC_BitmapSize
{
    uint32_t              IndexSubTableArrayOffset;
    uint32_t              IndexTablesSize;
    uint32_t              NumberOfIndexSubTables;
    uint32_t              ColorRef;
    EBLC_SbitLineMetrics  Hori;
    EBLC_SbitLineMetrics  Vert;
    uint16_t              StartGlyphIndex;
    uint16_t              EndGlyphIndex;
    uint8_t               PpemX;
    uint8_t               PpemY;
    uint8_t               BitDepth;
    int8_t                Flags;
};

struct EBLC_Header
{
    uint16_t 	        MajorVersion;
    uint16_t 	        MinorVersion;
    uint32_t 	        NumSizes;
    EBLC_BitmapSize 	BitmapSizes[];
};

// EBSC - Embedded bitmap scaling table
struct EBSC_Header
{
    uint16_t    MajorVersion;
    uint16_t    MinorVersion;
    uint32_t    NumSizes;
};

struct EBSC_BitmapScale
{
    EBLC_SbitLineMetrics  Hori;
    EBLC_SbitLineMetrics  Vert;
    uint8_t               PpemX;
    uint8_t               PpemY;
    uint8_t               SubstitutePpemX;
    uint8_t               SubstitutePpemY;
};

// cmap - Character map table
struct CMAP_EncodingRecord
{
    uint16_t  PlatformID;
    uint16_t  EncodingID;
    uint32_t  SubtableOffset;
};

struct CMAP_Header
{
    uint16_t             Version;
    uint16_t             NumTables;
    CMAP_EncodingRecord  EncodingRecords[];
};

// glyf - Glyphs
struct GLYPH_Header
{
    int16_t    NumberOfContours;
    int16_t    XMin;
    int16_t    YMin;
    int16_t    XMax;
    int16_t    YMax;
};

struct GLYPH_ContoursDescription
{
    uint16_t    EndPtsOfContours[];
};

struct GLYPH_InstructionsDescription
{
    uint16_t   InstructionLength;
    uint8_t    Instructions[];
};

struct GLYPH_FlagsDescription
{
    uint8_t    Flags[];
};

struct GLYPH_XCoordinatesDescription8
{
    uint8_t    XCoordinates[];
};

struct GLYPH_XCoordinatesDescription16
{
    uint16_t    XCoordinates[];
};

struct GLYPH_YCoordinatesDescription8
{
    uint8_t    YCoordinates[];
};

struct GLYPH_YCoordinatesDescription16
{
    uint16_t    YCoordinates[];
};

// HEAD - Font header table
struct HEAD_Header
{
    uint16_t    MajorVersion;
    uint16_t    MinorVersion;
    // There's more, but I don't know what "Fixed" means as a type :P
};

// HHEA - Font header table
struct HHEA_Header
{
    uint16_t    MajorVersion;
    uint16_t    MinorVersion;
    // There's more, but I don't know what "FWORD" means as a type :P
};

// LOCA - Index to location
struct LOCA_Header
{
    uint32_t    offsets[];
};

// MAXP - Maximum profile
struct MAXP_Header
{
    uint32_t    Version;
    uint16_t    NumGlyphs;
    uint16_t    MaxPoints;
    uint16_t    MaxContours;
    uint16_t    MaxCompositePoints;
    uint16_t    MaxCompositeContours;
    uint16_t    MaxZones;
    uint16_t    MaxTwilightPoints;
    uint16_t    MaxStorage;
    uint16_t    MaxFunctionDefs;
    uint16_t    MaxInstructionDefs;
    uint16_t    MaxStackElements;
    uint16_t    MaxSizeOfInstructions;
    uint16_t    MaxComponentElements;
    uint16_t    MaxComponentDepth;
};

#pragma pack(pop)

using namespace sputter::render;

// REMOVEME
#include <iostream>
// REMOVEME

TrueTypeParser::TrueTypeParser(const assets::BinaryData& dataToParse) 
    : m_isGood(false)
{
    const char* pDataStart = dataToParse.spData.get();
    const char* pData = pDataStart;
    size_t bytesRemaining = dataToParse.DataLength;
    const auto* pOffsetSubtable = ReadNext<OffsetSubtable>(&pData, &bytesRemaining);
    if (!pOffsetSubtable)
    {
        system::LogAndFail("TTF: Unexpected EOF");
    }
    
    if (SwapEndianness(pOffsetSubtable->ScalerType) != 0x00010000)
    {
        LOG(ERROR) << "TTF: Unsupported scaler type";
        return;
    }

    const uint32_t kMaxTableDirectories = 18;
    const uint32_t NumTables = SwapEndianness(pOffsetSubtable->NumTables);
    if (NumTables > kMaxTableDirectories)
    {
        system::LogAndFail("TTF: Too many table directories");
    }

    const EBLC_Header*  pEblcHeader = nullptr;
    const EBDT_Header*  pEbdtHeader = nullptr;
    const EBSC_Header*  pEbscHeader = nullptr;
    const CMAP_Header*  pCmapHeader = nullptr;
    const GLYPH_Header* pFirstGlyphHeader = nullptr;
    const HEAD_Header*  pHeadHeader = nullptr;
    const HHEA_Header*  pHheaHeader = nullptr;
    const LOCA_Header*  pLocaHeader = nullptr;
    const MAXP_Header*  pMaxpHeader = nullptr;
    for (uint16_t i = 0; i < NumTables; ++i)
    {
        const auto* pTableDirectory = ReadNext<TableDirectory>(&pData, &bytesRemaining);
        if (!pTableDirectory)
        {
            system::LogAndFail("TTF: Unexpected EOF");
        }

        switch(pTableDirectory->Tag)
        {
            case FOURCC("EBDT"):
            {
                std::cerr << "Locating EBDT... ";
                pEbdtHeader = reinterpret_cast<const EBDT_Header*>(pDataStart + SwapEndianness(pTableDirectory->Offset));

                // Straight up hardcoded in the spec
                if (SwapEndianness(pEbdtHeader->MajorVersion) != 2 ||
                    SwapEndianness(pEbdtHeader->MinorVersion) != 0)
                {
                    std::cerr << "EBDT or directory entry is malformed" << std::endl;
                    return;
                }
                std::cerr << "found" << std::endl;;
            }
            break;
            case FOURCC("EBLC"):
            {
                std::cerr << "Locating EBLC... ";
                pEblcHeader = reinterpret_cast<const EBLC_Header*>(pDataStart + SwapEndianness(pTableDirectory->Offset));

                // Straight up hardcoded in the spec
                if (SwapEndianness(pEblcHeader->MajorVersion) != 2 ||
                    SwapEndianness(pEblcHeader->MinorVersion) != 0)
                {
                    std::cerr << "EBLC or directory entry is malformed" << std::endl;
                    return;
                }
                std::cerr << "found." << std::endl;
            }
            break;
            case FOURCC("EBSC"):
            {
                std::cerr << "Locating EBSC... ";
                pEbscHeader = reinterpret_cast<const EBSC_Header*>(pDataStart + SwapEndianness(pTableDirectory->Offset));

                // Straight up hardcoded in the spec
                if (SwapEndianness(pEbscHeader->MajorVersion) != 2 ||
                    SwapEndianness(pEbscHeader->MinorVersion) != 0)
                {
                    std::cerr << "EBSC or directory entry is malformed" << std::endl;
                    return;
                }
                std::cerr << "found." << std::endl;
            }
            break;
            case FOURCC("GDEF"):
            {
                // Don't need attachment points or any such complexity for our purposes.
                std::cerr << "GDEF in table directory. Skipping." << std::endl;
            }
            break;
            case FOURCC("GPOS"):
            {
                // We support monospace fonts only, no need for this table.
                std::cerr << "GPOS in table directory. Skipping." << std::endl;
            }
            break;
            case FOURCC("GSUB"):
            {
                // For script-based glyphs. Skip!
                std::cerr << "GSUB in table directory. Skipping." << std::endl;
            }
            break;
            case FOURCC("OS/2"):
            {
                // No support for weighting characters. Skip.
                std::cerr << "OS/2 in table directory. Skipping." << std::endl;
            }
            break;
            case FOURCC("VDMX"):
            {
                // Still no support for weighting characters. Skip.
                std::cerr << "VDMX in table directory. Skipping." << std::endl;
            }
            break;
            case FOURCC("cmap"):
            {
                std::cerr << "Locating cmap... ";
                pCmapHeader = reinterpret_cast<const CMAP_Header*>(pDataStart + SwapEndianness(pTableDirectory->Offset));

                // Straight up hardcoded in the spec
                if (SwapEndianness(pCmapHeader->Version) != 0)
                {
                    std::cerr << "cmap or directory entry is malformed" << std::endl;
                    return;
                }

                std::cerr << "found." << std::endl;

                // REMOVE
                std::cerr << "Number of tables: " << SwapEndianness(pCmapHeader->NumTables) << std::endl;
            }
            break;
            case FOURCC("gasp"):
            {
                // We're strictly rendering binary glyphs.
                std::cerr << "gasp in table directory. Skipping." << std::endl;
            }
            break;
            case FOURCC("glyf"):
            {
                std::cerr << "Locating glyf... ";
                pFirstGlyphHeader = reinterpret_cast<const GLYPH_Header*>(pDataStart + SwapEndianness(pTableDirectory->Offset));

                std::cerr << "Number of contours: " << SwapEndianness((uint16_t)pFirstGlyphHeader->NumberOfContours) << std::endl;
                std::cerr << "MinX: " << SwapEndianness((uint16_t)pFirstGlyphHeader->XMin) << std::endl;
                std::cerr << "MaxX: " << SwapEndianness((uint16_t)pFirstGlyphHeader->XMax) << std::endl;
                std::cerr << "MinY: " << SwapEndianness((uint16_t)pFirstGlyphHeader->YMin) << std::endl;
                std::cerr << "MinY: " << SwapEndianness((uint16_t)pFirstGlyphHeader->YMax) << std::endl;

                std::cerr << "found." << std::endl;
            }
            break;
            case FOURCC("head"):
            {
                std::cerr << "Locating head... " << std::endl;
                pHeadHeader = reinterpret_cast<const HEAD_Header*>(pDataStart + SwapEndianness(pTableDirectory->Offset));
                if (SwapEndianness(pHeadHeader->MajorVersion) != 1 ||
                    SwapEndianness(pHeadHeader->MinorVersion) != 0)
                {
                    std::cerr << "head or directory entry is malformed" << std::endl;
                    return;
                }
                
                std::cerr << "found." << std::endl;
            }
            break;
            case FOURCC("hhea"):
            {
                std::cerr << "Locating hhea... " << std::endl;
                pHheaHeader = reinterpret_cast<const HHEA_Header*>(pDataStart + SwapEndianness(pTableDirectory->Offset));
                if (SwapEndianness(pHheaHeader->MajorVersion) != 1 ||
                    SwapEndianness(pHheaHeader->MinorVersion) != 0)
                {
                    std::cerr << "hhea or directory entry is malformed" << std::endl;
                    return;
                }
                
                std::cerr << "found." << std::endl;
            }
            break;
            case FOURCC("hmtx"):
            {
                // Unclear if I need to support this yet?
                std::cerr << "hmtx found in directory, but skipping." << std::endl;
            }
            break;
            case FOURCC("loca"):
            {
                std::cerr << "Locating loca table... ";
                pLocaHeader = reinterpret_cast<const LOCA_Header*>(pDataStart + SwapEndianness(pTableDirectory->Offset));
                std::cerr << "found." << std::endl;
            }
            break;
            case FOURCC("maxp"):
            {
                std::cerr << "Locating maxp table... ";
                pMaxpHeader = reinterpret_cast<const MAXP_Header*>(pDataStart + SwapEndianness(pTableDirectory->Offset));

                if (SwapEndianness(pMaxpHeader->Version) != 0x00010000)
                {
                    std::cerr << "maxp or directory entry is malformed" << std::endl;
                    return;
                }
                
                std::cerr << "found." << std::endl;
            }
            break;
            case FOURCC("name"):
            {
                // Nah. Skip.
                std::cerr << "name in table directory. Skipping." << std::endl;
            }
            break;
            case FOURCC("post"):
            {
                // Don't need this either. Skip.
                std::cerr << "post in table directory. Skipping." << std::endl;
            }
            break;
        }
    }

    m_isGood = true;
}

bool TrueTypeParser::IsGood() 
{
    return m_isGood;
}
