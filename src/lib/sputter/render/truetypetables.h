#pragma once

#include <cstdint>

// See spec @ https://docs.microsoft.com/en-us/typography/opentype/spec/ttch01
// Various table layouts @ https://docs.microsoft.com/en-us/typography/opentype/spec/glyf?source=recommendations
#pragma pack(push, 1)
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
const uint8_t kGLYPHPointFlagOnCurvePoint      = 1 << 0;
const uint8_t kGLYPHPointFlagXShortVector      = 1 << 1;
const uint8_t kGLYPHPointFlagYShortVector      = 1 << 2;
const uint8_t kGLYPHPointFlagRepeatFlags       = 1 << 3;
const uint8_t kGLYPHPointFlagXIsSameOrPositive = 1 << 4;
const uint8_t kGLYPHPointFlagYIsSameOrPositive = 1 << 5;
const uint8_t kGLYPHPointFlagOverlapSimple     = 1 << 6;

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
    int64_t     Created;
    int64_t     Modified;
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
