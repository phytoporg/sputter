#include "truetypeparser.h"

#include <cstdint>
#include <vector>

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

    template<typename ReturnType, typename OffsetStartType>
    const ReturnType* GetOffsetFrom(const OffsetStartType* pBase, uint32_t byteOffset)
    {
        return reinterpret_cast<const ReturnType*>(
            reinterpret_cast<const char*>(pBase) + byteOffset
        );
    }

    uint32_t SwapEndianness32(uint32_t value)
    {
        return
            (value << 24) |
            (value & 0xFF00) << 8 |
            ((value >> 8)  & 0xFF00) |
            ((value >> 24) & 0xFF);
    }

    uint16_t SwapEndianness16(uint16_t value)
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

// CMAP Encoding record platform IDs
const uint16_t kCMAPPlatformIdUnicode   = 0;
const uint16_t kCMAPPlatformIdMacintosh = 1;
const uint16_t kCMAPPlatformIdISO       = 2;
const uint16_t kCMAPPlatformIdWindows   = 3;
const uint16_t kCMAPPlatformIdCustom    = 4;

// CMAP Encoding record encoding IDs
const uint16_t kCMAPUnicodeBMPEncodingForWindows = 1; // The only encoding we support

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

struct CMAP_SegmentMapHeader // Format 4
{
    uint16_t    Format;
    uint16_t    Length;
    uint16_t    Language;
    uint16_t    SegCountX2;
    uint16_t    SearchRange;
    uint16_t    EntrySelector;
    uint16_t    RangeShift;
    uint16_t    EndCode[];
};

struct CMAP_SegmentMapDescription // Format 4
{
    uint16_t    Format;
    uint16_t    Length;
    uint16_t    Language;
    uint16_t    SegCountX2;
    uint16_t    SearchRange;
    uint16_t    EntrySelector;
    uint16_t    RangeShift;
};

// The actual layout looks like:
// CMAP_SegmentMapDescription, as above
// End codes (SegCount)
// Two bytes of zero-padding
// Start codes (xSegCount)
// IDDelts (xSegCount)
// IDRangeOffsets (xSegCount)
// GlyphIDArray (xArbitrary)

struct CMAP_SegmentMapPointers
{
    // These need to be set up, all pointing to SegCount elements
    const uint16_t* pEndCodes = nullptr;
    const uint16_t* pStartCodes = nullptr;
    const int16_t*  pIdDeltas = nullptr;
    const uint16_t* pIdRangeOffsets = nullptr;
    const uint16_t* pGlyphIdArray = nullptr;
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
    uint16_t   EndPtsOfContours[];
};

struct GLYPH_InstructionsDescription
{
    uint16_t   InstructionLength;
    uint8_t    pInstructions[];
};

// PointData flags
const uint8_t kGLYPHPointFlagOnCurvePoint      = 1 << 1;
const uint8_t kGLYPHPointFlagXShortVector      = 1 << 2;
const uint8_t kGLYPHPointFlagYShortVector      = 1 << 3;
const uint8_t kGLYPHPointFlagRepeatFlags       = 1 << 4;
const uint8_t kGLYPHPointFlagXIsSameOrPositive = 1 << 5;
const uint8_t kGLYPHPointFlagYIsSameOrPositive = 1 << 6;
const uint8_t kGLYPHPointFlagOverlapSimple     = 1 << 7;

struct GLYPH_PointData
{
    const uint8_t*  pFlags;
    const uint8_t*  pXCoordinates;
    const uint8_t*  pYCoordinates;
};

// HEAD - Font header table
struct HEAD_Header
{
    uint16_t    MajorVersion;
    uint16_t    MinorVersion;
    uint32_t    FontRevision; // "Fixed"?
    uint32_t    ChecksumAdjustment;
    uint32_t    MagicNumber;
    uint16_t    Flags;
    uint16_t    UnitsPerEm;
    uint32_t    Created;
    uint32_t    Modified;
    int16_t     XMin;
    int16_t     YMin;
    int16_t     XMax;
    int16_t     YMax;
    uint16_t    MacStyle;
    uint16_t    LowestRecPPEM;
    int16_t     FontDirectionHint;
    int16_t     IndexToLocFormat;
    int16_t     GlyphDataFormat;
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
    uint32_t    Offsets[];
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
    
    if (SwapEndianness32(pOffsetSubtable->ScalerType) != 0x00010000)
    {
        LOG(ERROR) << "TTF: Unsupported scaler type";
        return;
    }

    const uint32_t kMaxTableDirectories = 18;
    const uint16_t NumTables = SwapEndianness16(pOffsetSubtable->NumTables);
    if (NumTables > kMaxTableDirectories)
    {
        system::LogAndFail("TTF: Too many table directories");
    }

    // For use in processing contour points
    std::vector<uint8_t> expandedContourPointFlags;
    std::vector<uint8_t> expandedContourXCoordinates;
    std::vector<uint8_t> expandedContourYCoordinates;

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
                LOG(INFO) << "Locating EBDT... ";
                pEbdtHeader = reinterpret_cast<const EBDT_Header*>(pDataStart + SwapEndianness32(pTableDirectory->Offset));

                // Straight up hardcoded in the spec
                if (SwapEndianness16(pEbdtHeader->MajorVersion) != 2 ||
                    SwapEndianness16(pEbdtHeader->MinorVersion) != 0)
                {
                    LOG(ERROR) << "EBDT or directory entry is malformed";
                    return;
                }
                LOG(INFO) << "found";
            }
            break;
            case FOURCC("EBLC"):
            {
                LOG(INFO) << "Locating EBLC... ";
                pEblcHeader = reinterpret_cast<const EBLC_Header*>(pDataStart + SwapEndianness32(pTableDirectory->Offset));

                // Straight up hardcoded in the spec
                if (SwapEndianness16(pEblcHeader->MajorVersion) != 2 ||
                    SwapEndianness16(pEblcHeader->MinorVersion) != 0)
                {
                    LOG(ERROR) << "EBLC or directory entry is malformed";
                    return;
                }
                LOG(INFO) << "found.";
            }
            break;
            case FOURCC("EBSC"):
            {
                LOG(INFO) << "Locating EBSC... ";
                pEbscHeader = reinterpret_cast<const EBSC_Header*>(pDataStart + SwapEndianness32(pTableDirectory->Offset));

                // Straight up hardcoded in the spec
                if (SwapEndianness16(pEbscHeader->MajorVersion) != 2 ||
                    SwapEndianness16(pEbscHeader->MinorVersion) != 0)
                {
                    LOG(ERROR) << "EBSC or directory entry is malformed";
                    return;
                }
                LOG(INFO) << "found.";
            }
            break;
            case FOURCC("GDEF"):
            {
                // Don't need attachment points or any such complexity for our purposes.
                LOG(INFO) << "GDEF in table directory. Skipping.";
            }
            break;
            case FOURCC("GPOS"):
            {
                // We support monospace fonts only, no need for this table.
                LOG(INFO) << "GPOS in table directory. Skipping.";
            }
            break;
            case FOURCC("GSUB"):
            {
                // For script-based glyphs. Skip!
                LOG(INFO) << "GSUB in table directory. Skipping.";
            }
            break;
            case FOURCC("OS/2"):
            {
                // No support for weighting characters. Skip.
                LOG(INFO) << "OS/2 in table directory. Skipping.";
            }
            break;
            case FOURCC("VDMX"):
            {
                // Still no support for weighting characters. Skip.
                LOG(INFO) << "VDMX in table directory. Skipping.";
            }
            break;
            case FOURCC("cmap"):
            {
                LOG(INFO) << "Locating cmap... ";
                pCmapHeader = reinterpret_cast<const CMAP_Header*>(pDataStart + SwapEndianness32(pTableDirectory->Offset));

                // Straight up hardcoded in the spec
                if (SwapEndianness16(pCmapHeader->Version) != 0)
                {
                    LOG(ERROR) << "cmap or directory entry is malformed";
                    return;
                }

                LOG(INFO) << "found.";
            }
            break;
            case FOURCC("gasp"):
            {
                // We're strictly rendering binary glyphs.
                LOG(INFO) << "gasp in table directory. Skipping.";
            }
            break;
            case FOURCC("glyf"):
            {
                LOG(INFO) << "Locating glyf... ";
                pFirstGlyphHeader = reinterpret_cast<const GLYPH_Header*>(pDataStart + SwapEndianness32(pTableDirectory->Offset));
                LOG(INFO) << "found.";
            }
            break;
            case FOURCC("head"):
            {
                LOG(INFO) << "Locating head... ";
                pHeadHeader = reinterpret_cast<const HEAD_Header*>(pDataStart + SwapEndianness32(pTableDirectory->Offset));
                if (SwapEndianness16(pHeadHeader->MajorVersion) != 1 ||
                    SwapEndianness16(pHeadHeader->MinorVersion) != 0 ||
                    SwapEndianness32(pHeadHeader->MagicNumber)  != 0x5F0F3CF5)
                {
                    LOG(ERROR) << "head or directory entry is malformed";
                    return;
                }

                LOG(INFO) << "found.";
            }
            break;
            case FOURCC("hhea"):
            {
                LOG(INFO) << "Locating hhea... ";
                pHheaHeader = reinterpret_cast<const HHEA_Header*>(pDataStart + SwapEndianness32(pTableDirectory->Offset));
                if (SwapEndianness16(pHheaHeader->MajorVersion) != 1 ||
                    SwapEndianness16(pHheaHeader->MinorVersion) != 0)
                {
                    LOG(ERROR) << "hhea or directory entry is malformed";
                    return;
                }
                
                LOG(INFO) << "found.";
            }
            break;
            case FOURCC("hmtx"):
            {
                // Unclear if this is needed just yet. Skip for now.
                LOG(INFO) << "hmtx found in directory, but skipping.";
            }
            break;
            case FOURCC("loca"):
            {
                LOG(INFO) << "Locating loca table... ";
                pLocaHeader = reinterpret_cast<const LOCA_Header*>(pDataStart + SwapEndianness32(pTableDirectory->Offset));
                LOG(INFO) << "found.";
            }
            break;
            case FOURCC("maxp"):
            {
                LOG(INFO) << "Locating maxp table... ";
                pMaxpHeader = reinterpret_cast<const MAXP_Header*>(pDataStart + SwapEndianness32(pTableDirectory->Offset));

                if (SwapEndianness32(pMaxpHeader->Version) != 0x00010000)
                {
                    LOG(ERROR) << "maxp or directory entry is malformed";
                    return;
                }

                expandedContourPointFlags.resize(pMaxpHeader->MaxPoints);
                expandedContourXCoordinates.resize(pMaxpHeader->MaxPoints);
                expandedContourYCoordinates.resize(pMaxpHeader->MaxPoints);
                
                LOG(INFO) << "found.";
            }
            break;
            case FOURCC("name"):
            {
                // Nah. Skip.
                LOG(INFO) << "name in table directory. Skipping.";
            }
            break;
            case FOURCC("post"):
            {
                // Don't need this either. Skip.
                LOG(INFO) << "post in table directory. Skipping.";
            }
            break;
        }
    }

    // Okay great. Now we have some headers. Let's check out cmap
    const uint16_t NumCmapTables = SwapEndianness16(pCmapHeader->NumTables);
    const CMAP_SegmentMapHeader* pSegmentMap = nullptr;
    for (uint16_t i = 0; i < NumCmapTables; ++i)
    {
        const CMAP_EncodingRecord* pCurrentEncodingRecord = &pCmapHeader->EncodingRecords[i];
        const uint16_t PlatformID = SwapEndianness16(pCurrentEncodingRecord->PlatformID);
        const uint16_t EncodingID = SwapEndianness16(pCurrentEncodingRecord->EncodingID);

        if (PlatformID == kCMAPPlatformIdWindows && EncodingID == kCMAPUnicodeBMPEncodingForWindows)
        {
            // This is the configuration we're after. Solid.
            const uint32_t MapOffset = SwapEndianness32(pCurrentEncodingRecord->SubtableOffset);
            pSegmentMap = GetOffsetFrom<CMAP_SegmentMapHeader>(pCmapHeader, MapOffset);
            break;
        }
    }

    if (!pSegmentMap)
    {
        LOG(ERROR) << "TTF: Unsupported encoding format";
        return;
    }
    
    const uint32_t NumSegmentsX2 = SwapEndianness16(pSegmentMap->SegCountX2);
    const uint32_t NumSegments = NumSegmentsX2 / 2;
    CMAP_SegmentMapPointers segmentMapPointers;
    segmentMapPointers.pEndCodes       = GetOffsetFrom<uint16_t>(pSegmentMap, sizeof(*pSegmentMap));
    segmentMapPointers.pStartCodes     = GetOffsetFrom<uint16_t>(segmentMapPointers.pEndCodes, NumSegmentsX2 + 2);
    segmentMapPointers.pIdDeltas       = GetOffsetFrom<int16_t>(segmentMapPointers.pStartCodes, NumSegmentsX2);
    segmentMapPointers.pIdRangeOffsets = GetOffsetFrom<uint16_t>(segmentMapPointers.pIdDeltas, NumSegmentsX2);
    segmentMapPointers.pGlyphIdArray   = GetOffsetFrom<uint16_t>(segmentMapPointers.pIdRangeOffsets, NumSegmentsX2);

    if (segmentMapPointers.pEndCodes[NumSegments] != 0 ||
        segmentMapPointers.pEndCodes[NumSegments - 1] != 0xFFFF)
    {
        system::LogAndFail("Improperly initialized segment map pointers");
    }

    // Linear search for a test character. The segment map is designed for binary search--
    // later !
    const uint16_t TestChar = static_cast<uint16_t>('0');
    uint16_t glyphId = 0xFFFF; // Invalid
    for (uint16_t i = 0; i < NumSegments; ++i)
    {
        const uint16_t SegmentStart = SwapEndianness16(segmentMapPointers.pStartCodes[i]);
        const uint16_t SegmentEnd = SwapEndianness16(segmentMapPointers.pEndCodes[i]);

        if (TestChar <= SegmentEnd && TestChar >= SegmentStart)
        {
            const uint16_t IdOffset = SwapEndianness16(segmentMapPointers.pIdRangeOffsets[i]);

            // Indexing trick from the spec
            const uint16_t GlyphIndex = SwapEndianness16(*(IdOffset / 2 + (TestChar - SegmentStart) + &segmentMapPointers.pIdRangeOffsets[i]));
            if (segmentMapPointers.pGlyphIdArray[GlyphIndex] == 0)
            {
                glyphId = TestChar + SwapEndianness16(segmentMapPointers.pIdDeltas[i]);
            }
            else
            {
                glyphId = SwapEndianness16(GlyphIndex);
            }
        }
        else if (TestChar > SegmentEnd || SegmentEnd == 0xFFFF)
        {
            break;
        }
    }

    // Grab the offset for this glyph
    const uint32_t GlyphOffset = SwapEndianness32(pLocaHeader->Offsets[glyphId]);
    const GLYPH_Header* pFoundGlyphHeader = GetOffsetFrom<GLYPH_Header>(pFirstGlyphHeader, GlyphOffset);
    if (SwapEndianness16(pFoundGlyphHeader->NumberOfContours) < 0)
    {
        LOG(ERROR) << "Unsupported glyph description type";
        return;
    }

    const uint16_t NumberOfContours = SwapEndianness16(pFoundGlyphHeader->NumberOfContours);
    const GLYPH_ContoursDescription* pContourDescriptions = GetOffsetFrom<GLYPH_ContoursDescription>(pFoundGlyphHeader, sizeof(*pFoundGlyphHeader));
    const GLYPH_InstructionsDescription* pInstructionDescriptions = GetOffsetFrom<GLYPH_InstructionsDescription>(pContourDescriptions, NumberOfContours * 2);

    const uint16_t NumberOfPoints = SwapEndianness16(pContourDescriptions->EndPtsOfContours[NumberOfContours - 1]);
    GLYPH_PointData pointData;
    pointData.pFlags = GetOffsetFrom<uint8_t>(pInstructionDescriptions, pInstructionDescriptions->InstructionLength + 2);

    // The flags array can specify repeated flag bytes, so it can actually be shorter in length than the X/Y
    // coordinate array. This also means it must be processed to know the *actual* length of the array, which
    // we need to know where to begin processing X-coordinates.

    expandedContourPointFlags.resize(NumberOfPoints);
    const uint8_t* pCurrentFlag = pointData.pFlags;
    uint16_t logicalFlagEntry = 0;
    while (logicalFlagEntry < NumberOfPoints)
    {
        const uint8_t Flags = pointData.pFlags[logicalFlagEntry];
        expandedContourPointFlags[logicalFlagEntry] = Flags;
        ++pCurrentFlag;

        if (Flags & kGLYPHPointFlagRepeatFlags)
        {
            // The next byte dictates how many times this value is repeated.
            // See: https://docs.microsoft.com/en-us/typography/opentype/spec/glyf?source=recommendations
            uint8_t repeatCount = pointData.pFlags[logicalFlagEntry + 1];
            while (repeatCount--)
            {
                expandedContourPointFlags[++logicalFlagEntry] = Flags;
            }

            // Advance from the repeat count
            ++pCurrentFlag;
        }
    }

    // Need to treat coordinates similarly! Coordinate components can be one or two bytes, depending on flags.
    pointData.pXCoordinates = pCurrentFlag;
    expandedContourXCoordinates.resize(NumberOfPoints);

    const uint8_t* pCurrentXCoordinate = pointData.pXCoordinates;
    uint16_t logicalXCoordinateEntry = 0;
    int16_t  previousXCoordinate = 0;
    while (logicalXCoordinateEntry < NumberOfPoints)
    {
        int16_t currentXCoordinate;
        const uint8_t Flags = expandedContourPointFlags[logicalXCoordinateEntry];
        if (Flags & kGLYPHPointFlagXShortVector)
        {
            currentXCoordinate = static_cast<int16_t>(*pCurrentXCoordinate);
            pCurrentXCoordinate++;

            if (!(Flags & kGLYPHPointFlagXIsSameOrPositive))
            {
                currentXCoordinate = -currentXCoordinate;
            }
        }
        else
        {
            if (Flags & kGLYPHPointFlagXIsSameOrPositive)
            {
                // Don't advance the X-coordinate index, this value is just getting repeated.
                currentXCoordinate = previousXCoordinate;
            }
            else
            {
                currentXCoordinate = SwapEndianness16(*reinterpret_cast<const int16_t*>(pCurrentXCoordinate));
                pCurrentXCoordinate += 2;
            }
        }

        // Quick sanity check
        if (currentXCoordinate < SwapEndianness16(pFoundGlyphHeader->XMin) || 
            currentXCoordinate > SwapEndianness16(pFoundGlyphHeader->XMax))
        {
            LOG(ERROR) << "Glyph X-coordinate out of bounds";
            return;
        }

        previousXCoordinate = currentXCoordinate;
        logicalXCoordinateEntry++;
    }

    // TODO: Y-coordinates!
    pointData.pYCoordinates = pCurrentXCoordinate;

#if 0
    // Iterate over glyphs
    const GLYPH_Header* pCurrentGlyphHeader = pFirstGlyphHeader;
    const uint16_t NumGlyphs = SwapEndianness16(pMaxpHeader->NumGlyphs);
    for (uint16_t i = 0; i < NumGlyphs; ++i)
    {
        const uint32_t GlyphOffset = SwapEndianness(pLocaHeader->offsets[i]);
        pSegmentMap = GetOffsetFrom<GLYPH_Header>(pFirstGlyphHeader, GlyphOffset);
    }
#endif

    m_isGood = true;
}

bool TrueTypeParser::IsGood() 
{
    return m_isGood;
}
