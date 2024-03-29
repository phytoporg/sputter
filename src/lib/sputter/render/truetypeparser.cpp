#include "truetypeparser.h"
#include "truetypetables.h"
#include "rasterizer.h"

#include <cstdint>
#include <cstring>
#include <vector>

#include <sputter/assets/binarydata.h>
#include <sputter/core/check.h>
#include <sputter/system/system.h>
#include <sputter/log/log.h>

// Placeholder! Gotta actually read this value from the horizontal metrics table.
const uint32_t kPlaceholderBearingX = 1;

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

    // Wanted to get away from templates, but we're committed.
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

    template<typename T>
    T SwapEndianness32(T value)
    {
        static_assert(sizeof(T) == 4, "Unexpected type size");
        return
            (value << 24) |
            (value & 0xFF00) << 8 |
            ((value >> 8)  & 0xFF00) |
            ((value >> 24) & 0xFF);
    }

    template<typename T>
    T SwapEndianness16(T value)
    {
        static_assert(sizeof(T) == 2, "Unexpected type size");
        return
            (value << 8) |
            ((value >> 8) & 0xFF);
    }

    // Process a coordinate array from glyph tables
    bool 
    DecodeGlyphCoordinates(
        const uint8_t* pCoordinateData,
        uint16_t numPoints,
        uint8_t shortVectorFlag,
        uint8_t isSameOrPositiveFlag,
        int16_t coordinateMin,
        int16_t coordinateMax,
        const std::vector<uint8_t>& flagsVector,
        std::vector<int16_t>* pExpandedVectorOut,
        const uint8_t** ppEndPointerOut = nullptr
    )
    {
        pExpandedVectorOut->resize(numPoints);

        const uint8_t* pCurrentCoordinate = pCoordinateData;
        uint16_t logicalCoordinateEntry = 0;
        int16_t  previousCoordinate = 0;
        while (logicalCoordinateEntry < numPoints)
        {
            int16_t currentCoordinate;
            const uint8_t Flags = flagsVector[logicalCoordinateEntry];
            if (Flags & shortVectorFlag)
            {
                // These are delta-encoded
                currentCoordinate = *pCurrentCoordinate;
                pCurrentCoordinate++;

                if (!(Flags & isSameOrPositiveFlag))
                {
                    // Encodes a delta
                    currentCoordinate = previousCoordinate - currentCoordinate;
                }
                else
                {
                    // Also encodes a delta, but don't negate
                    currentCoordinate += previousCoordinate;
                }
            }
            else
            {
                if (Flags & isSameOrPositiveFlag)
                {
                    // Does not encode a delta
                    currentCoordinate = previousCoordinate;
                }
                else
                {
                    // Encodes a delta
                    currentCoordinate = SwapEndianness16(*reinterpret_cast<const int16_t*>(pCurrentCoordinate)) + previousCoordinate;
                    pCurrentCoordinate += 2;
                }
            }

            (*pExpandedVectorOut)[logicalCoordinateEntry] = currentCoordinate;

            // Quick sanity check
            if (currentCoordinate < coordinateMin || 
                currentCoordinate > coordinateMax)
            {
                RELEASE_LOGLINE_ERROR(LOG_FONT, "Glyph coordinates out of bounds (%hu)", currentCoordinate);
                return false;
            }

            previousCoordinate = currentCoordinate;
            logicalCoordinateEntry++;
        }

        if (ppEndPointerOut)
        {
            *ppEndPointerOut = pCurrentCoordinate;
        }

        return true;
    }

    void DrawContourSegment(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t* pData, uint16_t stride, uint8_t color)
    {
        const uint8_t PreviousPointValue = pData[y0 * stride + x0];

        const uint8_t FillColor = (sputter::render::SegmentToRasterFlags(x0, y0, x1, y1) << 4) | color;
        sputter::render::DrawLine(x0, y0, x1, y1, pData, stride, FillColor);

        // OR in flags for the initial point, or we clobber correct directional information
        // for determining the winding number.
        pData[y0 * stride + x0] |= (PreviousPointValue & 0xF0);
    }

    void DebugDumpGlyph(uint8_t* pPixelGlyph, uint16_t width, uint16_t height)
    {
#if DEBUG
        for (int16_t y = height - 1; y >= 0; --y)
        {
            for (int16_t x = 0; x < width; ++x)
            {
                DEBUG_LOG_VERBOSE(LOG_FONT, "%s", (pPixelGlyph[y * width + x] & 1 ? "1" : "0"));
            }
            DEBUG_LOG_VERBOSE(LOG_FONT, "\n");
        }

        // Flush?
        DEBUG_LOG_VERBOSE(LOG_FONT, "\n");
#endif
    }
}

using namespace sputter::render;

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

    const uint32_t ScalerType = SwapEndianness32(pOffsetSubtable->ScalerType);
    if (ScalerType != 0x00010000)
    {
        RELEASE_LOGLINE_ERROR(LOG_FONT, "TTF: Unsupported scaler type: 0x%x", ScalerType);
        return;
    }

    const uint32_t kMaxTableDirectories = 18;
    const uint16_t NumTables = SwapEndianness16(pOffsetSubtable->NumTables);
    if (NumTables > kMaxTableDirectories)
    {
        system::LogAndFail("TTF: Too many table directories");
    }

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
                RELEASE_LOG_INFO(LOG_FONT, "Locating EBDT...");
                m_pEbdtHeader = reinterpret_cast<const EBDT_Header*>(pDataStart + SwapEndianness32(pTableDirectory->Offset));

                // Straight up hardcoded in the spec
                if (SwapEndianness16(m_pEbdtHeader->MajorVersion) != 2 ||
                    SwapEndianness16(m_pEbdtHeader->MinorVersion) != 0)
                {
                    RELEASE_LOGLINE_ERROR(LOG_FONT, "EBDT or directory entry is malformed");
                    return;
                }
                RELEASE_LOG_INFO(LOG_FONT, "found");
            }
            break;
            case FOURCC("EBLC"):
            {
                RELEASE_LOG_INFO(LOG_FONT, "Locating EBLC...");
                m_pEblcHeader = reinterpret_cast<const EBLC_Header*>(pDataStart + SwapEndianness32(pTableDirectory->Offset));

                // Straight up hardcoded in the spec
                if (SwapEndianness16(m_pEblcHeader->MajorVersion) != 2 ||
                    SwapEndianness16(m_pEblcHeader->MinorVersion) != 0)
                {
                    RELEASE_LOGLINE_ERROR(LOG_FONT, "EBLC or directory entry is malformed");
                    return;
                }
                RELEASE_LOG_INFO(LOG_FONT, "found");
            }
            break;
            case FOURCC("EBSC"):
            {
                RELEASE_LOG_INFO(LOG_FONT, "Locating EBSC...");
                m_pEbscHeader = reinterpret_cast<const EBSC_Header*>(pDataStart + SwapEndianness32(pTableDirectory->Offset));

                // Straight up hardcoded in the spec
                if (SwapEndianness16(m_pEbscHeader->MajorVersion) != 2 ||
                    SwapEndianness16(m_pEbscHeader->MinorVersion) != 0)
                {
                    RELEASE_LOGLINE_ERROR(LOG_FONT, "EBSC or directory entry is malformed");
                    return;
                }
                RELEASE_LOG_INFO(LOG_FONT, "found");
            }
            break;
            case FOURCC("GDEF"):
            {
                // Don't need attachment points or any such complexity for our purposes.
                RELEASE_LOGLINE_INFO(LOG_FONT, "GDEF in table directory. Skipping.");
            }
            break;
            case FOURCC("GPOS"):
            {
                // We support monospace fonts only, no need for this table.
                RELEASE_LOGLINE_INFO(LOG_FONT, "GPOS in table directory. Skipping.");
            }
            break;
            case FOURCC("GSUB"):
            {
                // For script-based glyphs. Skip!
                RELEASE_LOGLINE_INFO(LOG_FONT, "GSUB in table directory. Skipping.");
            }
            break;
            case FOURCC("OS/2"):
            {
                // No support for weighting characters. Skip.
                RELEASE_LOGLINE_INFO(LOG_FONT, "OS/2 in table directory. Skipping.");
            }
            break;
            case FOURCC("VDMX"):
            {
                // Still no support for weighting characters. Skip.
                RELEASE_LOGLINE_INFO(LOG_FONT, "VDMX in table directory. Skipping.");
            }
            break;
            case FOURCC("cmap"):
            {
                RELEASE_LOG_INFO(LOG_FONT, "Locating cmap...");
                m_pCmapHeader = reinterpret_cast<const CMAP_Header*>(pDataStart + SwapEndianness32(pTableDirectory->Offset));

                // Straight up hardcoded in the spec
                if (SwapEndianness16(m_pCmapHeader->Version) != 0)
                {
                    RELEASE_LOGLINE_ERROR(LOG_FONT, "cmap or directory entry is malformed");
                    return;
                }

                RELEASE_LOG_INFO(LOG_FONT, "found");
            }
            break;
            case FOURCC("gasp"):
            {
                // We're strictly rendering binary glyphs.
                RELEASE_LOGLINE_INFO(LOG_FONT, "gasp in table directory. Skipping.");
            }
            break;
            case FOURCC("glyf"):
            {
                RELEASE_LOG_INFO(LOG_FONT, "Locating glyf...");
                m_pFirstGlyphHeader = reinterpret_cast<const GLYPH_Header*>(pDataStart + SwapEndianness32(pTableDirectory->Offset));
                RELEASE_LOG_INFO(LOG_FONT, "found.");
            }
            break;
            case FOURCC("head"):
            {
                RELEASE_LOG_INFO(LOG_FONT, "Locating head...");
                m_pHeadHeader = reinterpret_cast<const HEAD_Header*>(pDataStart + SwapEndianness32(pTableDirectory->Offset));
                if (SwapEndianness16(m_pHeadHeader->MajorVersion) != 1 ||
                    SwapEndianness16(m_pHeadHeader->MinorVersion) != 0 ||
                    SwapEndianness32(m_pHeadHeader->MagicNumber)  != 0x5F0F3CF5)
                {
                    RELEASE_LOGLINE_ERROR(LOG_FONT, "head or directory entry is malformed");
                    return;
                }

                RELEASE_LOG_INFO(LOG_FONT, "found");
            }
            break;
            case FOURCC("hhea"):
            {
                RELEASE_LOG_INFO(LOG_FONT, "Locating hhea...");
                m_pHheaHeader = reinterpret_cast<const HHEA_Header*>(
                        pDataStart + SwapEndianness32(pTableDirectory->Offset));
                if (SwapEndianness32(m_pHheaHeader->Version) != 0x00010000)
                {
                    RELEASE_LOGLINE_ERROR(LOG_FONT, "hhea or directory entry is malformed");
                    return;
                }
                
                RELEASE_LOG_INFO(LOG_FONT, "found.");
            }
            case FOURCC("vhea"):
            {
                RELEASE_LOG_INFO(LOG_FONT, "Locating vhea...");
                m_pVheaHeader = reinterpret_cast<const VHEA_Header*>(
                        pDataStart + SwapEndianness32(pTableDirectory->Offset));
                if (SwapEndianness32(m_pVheaHeader->Version) != 0x00010000)
                {
                    RELEASE_LOGLINE_ERROR(LOG_FONT, "vhea or directory entry is malformed");
                    return;
                }
                
                RELEASE_LOG_INFO(LOG_FONT, "found");
            }
            break;
            case FOURCC("hmtx"):
            {
                RELEASE_LOG_INFO(LOG_FONT, "Locating hmtx...");
                m_pHmtx = reinterpret_cast<const HMTX*>(
                        pDataStart + SwapEndianness32(pTableDirectory->Offset));
                RELEASE_LOG_INFO(LOG_FONT, "found.");
            }
            case FOURCC("vmtx"):
            {
                RELEASE_LOG_INFO(LOG_FONT, "Locating vmtx...");
                m_pVmtx = reinterpret_cast<const VMTX*>(
                        pDataStart + SwapEndianness32(pTableDirectory->Offset));
                RELEASE_LOG_INFO(LOG_FONT, "found.");
            }
            break;
            case FOURCC("loca"):
            {
                RELEASE_LOG_INFO(LOG_FONT, "Locating loca table...");
                m_pLocaHeader = reinterpret_cast<const LOCA_Header*>(pDataStart + SwapEndianness32(pTableDirectory->Offset));
                RELEASE_LOG_INFO(LOG_FONT, "found.");
            }
            break;
            case FOURCC("maxp"):
            {
                RELEASE_LOG_INFO(LOG_FONT, "Locating maxp...");
                m_pMaxpHeader = reinterpret_cast<const MAXP_Header*>(pDataStart + SwapEndianness32(pTableDirectory->Offset));

                if (SwapEndianness32(m_pMaxpHeader->Version) != 0x00010000)
                {
                    RELEASE_LOGLINE_ERROR(LOG_FONT, "maxp or directory entry is malformed");
                    return;
                }

                RELEASE_LOG_INFO(LOG_FONT, "found.");
            }
            break;
            case FOURCC("name"):
            {
                // Nah. Skip.
                RELEASE_LOGLINE_INFO(LOG_FONT, "name in table directory. Skipping.");
            }
            break;
            case FOURCC("post"):
            {
                // Don't need this either. Skip.
                RELEASE_LOGLINE_INFO(LOG_FONT, "post in table directory. Skipping.");
            }
            break;
            default:
                system::LogAndFail("Unhandled TrueType table");
                break;
        }
    }

    // Okay great. Now we have some headers. Let's check out cmap
    const uint16_t NumCmapTables = SwapEndianness16(m_pCmapHeader->NumTables);
    for (uint16_t i = 0; i < NumCmapTables; ++i)
    {
        const CMAP_EncodingRecord* pCurrentEncodingRecord = &m_pCmapHeader->EncodingRecords[i];
        const uint16_t PlatformID = SwapEndianness16(pCurrentEncodingRecord->PlatformID);
        const uint16_t EncodingID = SwapEndianness16(pCurrentEncodingRecord->EncodingID);

        if (PlatformID == kCMAPPlatformIdWindows && EncodingID == kCMAPUnicodeBMPEncodingForWindows)
        {
            // This is the configuration we're after. Solid.
            const uint32_t MapOffset = SwapEndianness32(pCurrentEncodingRecord->SubtableOffset);
            m_pCmapSegmentMap = GetOffsetFrom<CMAP_SegmentMapHeader>(m_pCmapHeader, MapOffset);
            break;
        }
    }

    if (!m_pCmapSegmentMap)
    {
        RELEASE_LOGLINE_ERROR(LOG_FONT, "TTF: Unsupported encoding format");
        return;
    }
    
    const uint32_t NumSegmentsX2 = SwapEndianness16(m_pCmapSegmentMap->SegCountX2);
    const uint32_t NumSegments = NumSegmentsX2 / 2;
    m_CmapSegmentMapPointers.pEndCodes       = GetOffsetFrom<uint16_t>(m_pCmapSegmentMap, sizeof(*m_pCmapSegmentMap));
    m_CmapSegmentMapPointers.pStartCodes     = GetOffsetFrom<uint16_t>(m_CmapSegmentMapPointers.pEndCodes, NumSegmentsX2 + 2);
    m_CmapSegmentMapPointers.pIdDeltas       = GetOffsetFrom<int16_t>(m_CmapSegmentMapPointers.pStartCodes, NumSegmentsX2);
    m_CmapSegmentMapPointers.pIdRangeOffsets = GetOffsetFrom<uint16_t>(m_CmapSegmentMapPointers.pIdDeltas, NumSegmentsX2);
    m_CmapSegmentMapPointers.pGlyphIdArray   = GetOffsetFrom<uint16_t>(m_CmapSegmentMapPointers.pIdRangeOffsets, NumSegmentsX2);

    if (m_CmapSegmentMapPointers.pEndCodes[NumSegments] != 0 ||
        m_CmapSegmentMapPointers.pEndCodes[NumSegments - 1] != 0xFFFF)
    {
        system::LogAndFail("Improperly initialized segment map pointers");
    }

    m_isGood = true;
}

bool TrueTypeParser::IsGood() 
{
    return m_isGood;
}

Glyph TrueTypeParser::GetCharacterGlyph(char c)
{
    const GLYPH_Header* pFoundGlyphHeader = FindGlyphHeader(c);
    const uint16_t NumberOfContours = SwapEndianness16(pFoundGlyphHeader->NumberOfContours);

    if (NumberOfContours < 0)
    {
        RELEASE_LOGLINE_ERROR(LOG_FONT, "Unsupported glyph description type");
        return Glyph::kInvalidGlyph;
    }
    else if (NumberOfContours == 0)
    {
        return Glyph{ GetCharacterGlyphMetrics(c), nullptr };
    }

    const uint16_t* pEndPtsOfContours = &pFoundGlyphHeader->EndPtsOfContours[0];
    const GLYPH_InstructionsDescription* pInstructionDescriptions =
        GetOffsetFrom<GLYPH_InstructionsDescription>(pEndPtsOfContours, NumberOfContours * 2);

    // Add one, since the entries in EndPtsOfContours represent point *indices*
    const uint16_t NumberOfPoints = SwapEndianness16(pEndPtsOfContours[NumberOfContours - 1]) + 1;
    GLYPH_PointData pointData;
    pointData.pFlags = GetOffsetFrom<uint8_t>(pInstructionDescriptions, pInstructionDescriptions->InstructionLength + 2);

    // The flags array can specify repeated flag bytes, so it can actually be shorter in length than the X/Y
    // coordinate array. This also means it must be processed to know the *actual* length of the array, which
    // we need to know where to begin processing coordinates.
    std::vector<uint8_t> expandedContourPointFlags;
    std::vector<int16_t> expandedContourXCoordinates;
    std::vector<int16_t> expandedContourYCoordinates;

    expandedContourPointFlags.resize(NumberOfPoints);
    const uint8_t* pCurrentFlag = pointData.pFlags;
    uint16_t logicalFlagEntry = 0;
    while (logicalFlagEntry < NumberOfPoints)
    {
        const uint8_t Flags = *pCurrentFlag;
        expandedContourPointFlags[logicalFlagEntry] = Flags;
        ++pCurrentFlag;
        ++logicalFlagEntry;

        if (Flags & kGLYPHPointFlagRepeatFlags)
        {
            // The next byte dictates how many times this value is repeated.
            // See: https://docs.microsoft.com/en-us/typography/opentype/spec/glyf?source=recommendations
            uint8_t repeatCount = *pCurrentFlag;
            while (repeatCount--)
            {
                expandedContourPointFlags[++logicalFlagEntry] = Flags;
            }

            // Advance from the repeat count
            ++pCurrentFlag;
        }
    }

    // Individual coordinate components can be one or two bytes, depending on flags,
    //  and are delta-encoded. Gotta process them in order.
    pointData.pXCoordinates = pCurrentFlag;
    if (!DecodeGlyphCoordinates(
            pointData.pXCoordinates,
            NumberOfPoints,
            kGLYPHPointFlagXShortVector,
            kGLYPHPointFlagXIsSameOrPositive,
            SwapEndianness16(pFoundGlyphHeader->XMin),
            SwapEndianness16(pFoundGlyphHeader->XMax),
            expandedContourPointFlags,
            &expandedContourXCoordinates,
            &pointData.pYCoordinates))
    {
        RELEASE_LOGLINE_ERROR(LOG_FONT, "TTF: Failed to decode glyph X coordinates");
        return Glyph::kInvalidGlyph;
    }

    if (!DecodeGlyphCoordinates(
            pointData.pYCoordinates,
            NumberOfPoints,
            kGLYPHPointFlagYShortVector,
            kGLYPHPointFlagYIsSameOrPositive,
            SwapEndianness16(pFoundGlyphHeader->YMin),
            SwapEndianness16(pFoundGlyphHeader->YMax),
            expandedContourPointFlags,
            &expandedContourYCoordinates))
    {
        RELEASE_LOGLINE_ERROR(LOG_FONT, "TTF: Failed to decode glyph y coordinates");
        return Glyph::kInvalidGlyph;
    }

    // Scale and then rasterize
    // Scale -> pointSize * resolution / ( 72 points per inch * units_per_em )
    const uint8_t PointSize = 24; // TODO: parameterize
    const uint16_t PPI = 100;
    const uint32_t PPEM = PointSize * PPI / 72.0f;
    const uint16_t UnitsPerEm = SwapEndianness16(m_pHeadHeader->UnitsPerEm);
    const float EmToPixels = static_cast<float>(PPEM) / UnitsPerEm;
    const int16_t xMin = SwapEndianness16(pFoundGlyphHeader->XMin) * EmToPixels;
    const int16_t xMax = SwapEndianness16(pFoundGlyphHeader->XMax) * EmToPixels;
    const int16_t yMin = SwapEndianness16(pFoundGlyphHeader->YMin) * EmToPixels;
    const int16_t yMax = SwapEndianness16(pFoundGlyphHeader->YMax) * EmToPixels;
    const uint16_t GlyphHeight = yMax - yMin + 1;
    const uint16_t GlyphWidth = xMax - xMin + 1;
    uint8_t* pPixelGlyph = new uint8_t[GlyphWidth * GlyphHeight];
    if (!pPixelGlyph)
    {
        system::LogAndFail("Allocation fialed. OOM.");
    }
    memset(pPixelGlyph, 0, GlyphWidth * GlyphHeight);

    // Render the contours

    // TODO: This loop sucks, make it better!!
    uint16_t contourStartingPointIndex = 0;
    int16_t previousX = expandedContourXCoordinates[0] * EmToPixels - xMin;
    int16_t previousY = expandedContourYCoordinates[0] * EmToPixels - yMin;

    uint16_t pointsInContour = SwapEndianness16(pEndPtsOfContours[0]) + 1;
    uint16_t pointIndex = 1;
    uint16_t contourIndex = 0;

    // Encode directional flags into the upper nibble of each set contour pixel
    // for winding number computations during scanline fill.
    const uint8_t Color = 1;
    while (contourIndex < NumberOfContours && pointIndex < NumberOfPoints)
    {
        const uint8_t Flags = expandedContourPointFlags[pointIndex];
        const int16_t X     = expandedContourXCoordinates[pointIndex] * EmToPixels - xMin;
        const int16_t Y     = expandedContourYCoordinates[pointIndex] * EmToPixels - yMin;

        if (!(Flags & kGLYPHPointFlagOnCurvePoint))
        {
            system::LogAndFail("No support for bezier fanciness (yet)");
        }

        DrawContourSegment(previousX, previousY, X, Y, pPixelGlyph, GlyphWidth, Color);
        DebugDumpGlyph(pPixelGlyph, GlyphWidth, GlyphHeight);

        if ((pointIndex - contourStartingPointIndex) == pointsInContour - 1)
        {
            ++contourIndex;

            // Close the contour
            uint16_t contourStartX = expandedContourXCoordinates[contourStartingPointIndex] * EmToPixels - xMin;
            uint16_t contourStartY = expandedContourYCoordinates[contourStartingPointIndex] * EmToPixels - yMin;
            DrawContourSegment(X, Y, contourStartX, contourStartY, pPixelGlyph, GlyphWidth, Color);
            DebugDumpGlyph(pPixelGlyph, GlyphWidth, GlyphHeight);

            // Begin the next contour
            contourStartingPointIndex = ++pointIndex;
            pointsInContour = SwapEndianness16(pEndPtsOfContours[contourIndex]) + 1 - contourStartingPointIndex;

            previousX = expandedContourXCoordinates[contourStartingPointIndex] * EmToPixels - xMin;
            previousY = expandedContourYCoordinates[contourStartingPointIndex] * EmToPixels - yMin;
        }
        else
        {
            previousX = X;
            previousY = Y;
        }

        ++pointIndex;
    }

    // Call ScanlineFill() for each y-value
    for (int16_t y = yMin; y <= yMax; y++)
    {
        RELEASE_LOGLINE_VERYVERBOSE(LOG_FONT, "y = %d", y);
        uint8_t* pScanline = &pPixelGlyph[y * GlyphWidth];

        const uint8_t FinalFillColor = 1;
        ScanlineFill(pScanline, GlyphWidth, FinalFillColor);
        DebugDumpGlyph(pPixelGlyph, GlyphWidth, GlyphHeight);
    }
    
    // Remove flag bits from glyph data and place it in the returned glyph
    const size_t GlyphArea = GlyphHeight * GlyphWidth;
    for (size_t i = 0; i < GlyphArea; ++i)
    {
        pPixelGlyph[i] &= 0x0F;
    }

    auto newGlyph = Glyph{
        GlyphMetrics{ kPlaceholderBearingX, GlyphWidth, GlyphHeight },
        pPixelGlyph 
    };

    return newGlyph;
}

GlyphMetrics TrueTypeParser::GetCharacterGlyphMetrics(char c)
{
    uint16_t glyphId;
    const GLYPH_Header* pFoundGlyphHeader = FindGlyphHeader(c, &glyphId);

    // Grab the offset for this glyph
    const uint8_t PointSize = 24; // TODO: parameterize
    const uint16_t PPI = 100;
    const uint32_t PPEM = PointSize * PPI / 72.0f;
    const uint16_t UnitsPerEm = SwapEndianness16(m_pHeadHeader->UnitsPerEm);
    const float EmToPixels = static_cast<float>(PPEM) / UnitsPerEm;

    if (pFoundGlyphHeader->NumberOfContours)
    {
        const int16_t xMin = SwapEndianness16(pFoundGlyphHeader->XMin) * EmToPixels;
        const int16_t xMax = SwapEndianness16(pFoundGlyphHeader->XMax) * EmToPixels;
        const int16_t yMin = SwapEndianness16(pFoundGlyphHeader->YMin) * EmToPixels;
        const int16_t yMax = SwapEndianness16(pFoundGlyphHeader->YMax) * EmToPixels;
        const uint16_t GlyphHeight = yMax - yMin + 1;
        const uint16_t GlyphWidth = xMax - xMin + 1;

        return GlyphMetrics{ kPlaceholderBearingX, GlyphWidth, GlyphHeight };
    }
    else
    {
        // Empty glyph! Fall back on H metrics tables and just provide a bearing.
        const uint16_t NumHMetrics = 
            SwapEndianness16(m_pHheaHeader->NumOfLongHorMetrics);
        const uint16_t MetricsIndex = NumHMetrics > 1 ? glyphId : 0;
        const HMTX_LongHorizontalMetrics* pHMetrics =
            reinterpret_cast<const HMTX_LongHorizontalMetrics*>(&m_pHmtx);

        const uint16_t LeftBearing =
            SwapEndianness16(pHMetrics[MetricsIndex].LeftSideBearing) * EmToPixels;
        const uint16_t GlyphWidth = 0; 
        const uint16_t GlyphHeight = 0;

        return GlyphMetrics{ LeftBearing, GlyphWidth, GlyphHeight };
    }
}

const GLYPH_Header* TrueTypeParser::FindGlyphHeader(char c, uint16_t* pGlyphIndexOut)
{
    const uint32_t NumCmapSegmentsX2 = SwapEndianness16(m_pCmapSegmentMap->SegCountX2);
    const uint32_t NumCmapSegments = NumCmapSegmentsX2 / 2;

    const uint16_t GlyphCharacter = static_cast<uint16_t>(c);
    uint16_t glyphId = 0xFFFF; // Invalid
    for (uint16_t i = 0; i < NumCmapSegments; ++i)
    {
        const CMAP_SegmentMapPointers& SegmentMapPtrs = m_CmapSegmentMapPointers;

        const uint16_t SegmentStart = SwapEndianness16(SegmentMapPtrs.pStartCodes[i]);
        const uint16_t SegmentEnd = SwapEndianness16(SegmentMapPtrs.pEndCodes[i]);

        if (GlyphCharacter <= SegmentEnd && GlyphCharacter >= SegmentStart)
        {
            const uint16_t IdOffset = 
                SwapEndianness16(SegmentMapPtrs.pIdRangeOffsets[i]);
            if (IdOffset == 0)
            {
                glyphId = 
                    GlyphCharacter + SwapEndianness16(SegmentMapPtrs.pIdDeltas[i]);
            }
            else
            {
                // Indexing trick from the spec
                const uint16_t GlyphIndex = SwapEndianness16(
                        *(IdOffset / 2 + (GlyphCharacter - SegmentStart) + 
                        &SegmentMapPtrs.pIdRangeOffsets[i]));
                glyphId = SwapEndianness16(GlyphIndex);
                
            }
            break;
        }
        else if (GlyphCharacter > SegmentEnd || SegmentEnd == 0xFFFF)
        {
            break;
        }
    }

    const uint32_t GlyphOffset = SwapEndianness32(m_pLocaHeader->Offsets[glyphId]);
    const GLYPH_Header* pFoundGlyphHeader =
        GetOffsetFrom<GLYPH_Header>(m_pFirstGlyphHeader, GlyphOffset);
    const uint32_t NextGlyphOffset = 
        SwapEndianness32(m_pLocaHeader->Offsets[glyphId + 1]);
    const GLYPH_Header* pNextGlyphHeader =
        GetOffsetFrom<GLYPH_Header>(m_pFirstGlyphHeader, NextGlyphOffset);

    if (pGlyphIndexOut)
    {
        *pGlyphIndexOut = glyphId;
    }

    if (pFoundGlyphHeader == pNextGlyphHeader)
    {
        // Empty character, return an empty glyph header.
        static const GLYPH_Header EmptyGlyphHeader = {};
        return &EmptyGlyphHeader;
    }

    return pFoundGlyphHeader;
}
