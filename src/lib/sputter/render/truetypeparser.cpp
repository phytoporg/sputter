#include "truetypeparser.h"
#include "truetypetables.h"
#include "rasterizer.h"

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
                LOG(ERROR) << "Glyph coordinate out of bounds";
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
                VLOG(1) << (pPixelGlyph[y * width + x] & 1 ? "1" : "0") << " ";
            }
            VLOG(1) << "\n";
        }

        // Flush?
        VLOG(1) << "\n";
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
                m_pEbdtHeader = reinterpret_cast<const EBDT_Header*>(pDataStart + SwapEndianness32(pTableDirectory->Offset));

                // Straight up hardcoded in the spec
                if (SwapEndianness16(m_pEbdtHeader->MajorVersion) != 2 ||
                    SwapEndianness16(m_pEbdtHeader->MinorVersion) != 0)
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
                m_pEblcHeader = reinterpret_cast<const EBLC_Header*>(pDataStart + SwapEndianness32(pTableDirectory->Offset));

                // Straight up hardcoded in the spec
                if (SwapEndianness16(m_pEblcHeader->MajorVersion) != 2 ||
                    SwapEndianness16(m_pEblcHeader->MinorVersion) != 0)
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
                m_pEbscHeader = reinterpret_cast<const EBSC_Header*>(pDataStart + SwapEndianness32(pTableDirectory->Offset));

                // Straight up hardcoded in the spec
                if (SwapEndianness16(m_pEbscHeader->MajorVersion) != 2 ||
                    SwapEndianness16(m_pEbscHeader->MinorVersion) != 0)
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
                m_pCmapHeader = reinterpret_cast<const CMAP_Header*>(pDataStart + SwapEndianness32(pTableDirectory->Offset));

                // Straight up hardcoded in the spec
                if (SwapEndianness16(m_pCmapHeader->Version) != 0)
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
                m_pFirstGlyphHeader = reinterpret_cast<const GLYPH_Header*>(pDataStart + SwapEndianness32(pTableDirectory->Offset));
                LOG(INFO) << "found.";
            }
            break;
            case FOURCC("head"):
            {
                LOG(INFO) << "Locating head... ";
                m_pHeadHeader = reinterpret_cast<const HEAD_Header*>(pDataStart + SwapEndianness32(pTableDirectory->Offset));
                if (SwapEndianness16(m_pHeadHeader->MajorVersion) != 1 ||
                    SwapEndianness16(m_pHeadHeader->MinorVersion) != 0 ||
                    SwapEndianness32(m_pHeadHeader->MagicNumber)  != 0x5F0F3CF5)
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
                m_pHheaHeader = reinterpret_cast<const HHEA_Header*>(pDataStart + SwapEndianness32(pTableDirectory->Offset));
                if (SwapEndianness16(m_pHheaHeader->MajorVersion) != 1 ||
                    SwapEndianness16(m_pHheaHeader->MinorVersion) != 0)
                {
                    LOG(ERROR) << "hhea or directory entry is malformed";
                    return;
                }
                
                LOG(INFO) << "found.";
            }
            break;
            case FOURCC("hmtx"):
            {
                // TODO: Process this table and surface proper horizontal metrics for glyphs
                LOG(INFO) << "hmtx found in directory, but skipping.";
            }
            break;
            case FOURCC("loca"):
            {
                LOG(INFO) << "Locating loca table... ";
                m_pLocaHeader = reinterpret_cast<const LOCA_Header*>(pDataStart + SwapEndianness32(pTableDirectory->Offset));
                LOG(INFO) << "found.";
            }
            break;
            case FOURCC("maxp"):
            {
                LOG(INFO) << "Locating maxp table... ";
                m_pMaxpHeader = reinterpret_cast<const MAXP_Header*>(pDataStart + SwapEndianness32(pTableDirectory->Offset));

                if (SwapEndianness32(m_pMaxpHeader->Version) != 0x00010000)
                {
                    LOG(ERROR) << "maxp or directory entry is malformed";
                    return;
                }

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
        LOG(ERROR) << "TTF: Unsupported encoding format";
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
    const uint32_t NumCmapSegmentsX2 = SwapEndianness16(m_pCmapSegmentMap->SegCountX2);
    const uint32_t NumCmapSegments = NumCmapSegmentsX2 / 2;

    const uint16_t GlyphCharacter = static_cast<uint16_t>(c);
    uint16_t glyphId = 0xFFFF; // Invalid
    for (uint16_t i = 0; i < NumCmapSegments; ++i)
    {
        const uint16_t SegmentStart = SwapEndianness16(m_CmapSegmentMapPointers.pStartCodes[i]);
        const uint16_t SegmentEnd = SwapEndianness16(m_CmapSegmentMapPointers.pEndCodes[i]);

        if (GlyphCharacter <= SegmentEnd && GlyphCharacter >= SegmentStart)
        {
            const uint16_t IdOffset = SwapEndianness16(m_CmapSegmentMapPointers.pIdRangeOffsets[i]);

            // Indexing trick from the spec
            const uint16_t GlyphIndex = SwapEndianness16(*(IdOffset / 2 + (GlyphCharacter - SegmentStart) + &m_CmapSegmentMapPointers.pIdRangeOffsets[i]));
            if (m_CmapSegmentMapPointers.pGlyphIdArray[GlyphIndex] == 0)
            {
                glyphId = GlyphCharacter + SwapEndianness16(m_CmapSegmentMapPointers.pIdDeltas[i]);
            }
            else
            {
                glyphId = SwapEndianness16(GlyphIndex);
            }
        }
        else if (GlyphCharacter > SegmentEnd || SegmentEnd == 0xFFFF)
        {
            break;
        }
    }

    // Grab the offset for this glyph
    const uint32_t GlyphOffset = SwapEndianness32(m_pLocaHeader->Offsets[glyphId]);
    const GLYPH_Header* pFoundGlyphHeader = GetOffsetFrom<GLYPH_Header>(m_pFirstGlyphHeader, GlyphOffset);
    const uint16_t NumberOfContours = SwapEndianness16(pFoundGlyphHeader->NumberOfContours);
    if (SwapEndianness16(NumberOfContours) < 0)
    {
        LOG(ERROR) << "Unsupported glyph description type";
        return Glyph::kInvalidGlyph;
    }

    const GLYPH_ContoursDescription* pContourDescriptions = GetOffsetFrom<GLYPH_ContoursDescription>(pFoundGlyphHeader, sizeof(*pFoundGlyphHeader));
    const GLYPH_InstructionsDescription* pInstructionDescriptions = GetOffsetFrom<GLYPH_InstructionsDescription>(pContourDescriptions, NumberOfContours * 2);

    // Add one, since the entries in EndPtsOfContours represent point *indices*
    const uint16_t NumberOfPoints = SwapEndianness16(pContourDescriptions->EndPtsOfContours[NumberOfContours - 1]) + 1;
    GLYPH_PointData pointData;
    pointData.pFlags = GetOffsetFrom<uint8_t>(pInstructionDescriptions, pInstructionDescriptions->InstructionLength + 2);

    std::vector<uint8_t> expandedContourPointFlags;
    std::vector<int16_t> expandedContourXCoordinates;
    std::vector<int16_t> expandedContourYCoordinates;

    // The flags array can specify repeated flag bytes, so it can actually be shorter in length than the X/Y
    // coordinate array. This also means it must be processed to know the *actual* length of the array, which
    // we need to know where to begin processing coordinates.

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
        LOG(ERROR) << "TTF: Failed to decode glyph X coordinates";
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
        LOG(ERROR) << "TTF: Failed to decode glyph y coordinates";
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

    uint16_t pointsInContour = SwapEndianness16(pContourDescriptions->EndPtsOfContours[0]) + 1;
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

        if ((pointIndex - contourStartingPointIndex) == pointsInContour - 1)
        {
            ++contourIndex;

            // Close the contour
            uint16_t contourStartX = expandedContourXCoordinates[contourStartingPointIndex] * EmToPixels - xMin;
            uint16_t contourStartY = expandedContourYCoordinates[contourStartingPointIndex] * EmToPixels - yMin;
            DrawContourSegment(X, Y, contourStartX, contourStartY, pPixelGlyph, GlyphWidth, Color);

            contourStartingPointIndex = pointIndex + 1;
            pointsInContour = SwapEndianness16(pContourDescriptions->EndPtsOfContours[contourIndex]) + 1 - contourStartingPointIndex;
        }

        previousX = X;
        previousY = Y;

        ++pointIndex;
    }

    DebugDumpGlyph(pPixelGlyph, GlyphWidth, GlyphHeight);

    // Call ScanlineFill() for each y-value
    for (int16_t y = yMin; y <= yMax; y++)
    {
        VLOG(1) << "y = " << y << "\n";
        uint8_t* pScanline = &pPixelGlyph[y * GlyphWidth];

        const uint8_t FinalFillColor = 1;
        ScanlineFill(pScanline, GlyphWidth, FinalFillColor);
        DebugDumpGlyph(pPixelGlyph, GlyphWidth, GlyphHeight);
    }
    
    bool* pGlyphBitMatrix = new bool[GlyphWidth * GlyphHeight];
    for (int16_t y = 0; y < GlyphHeight; ++y) // One flat loop instead maybe?
    {
        for (int16_t x = 0; x < GlyphWidth; ++x)
        {
            const size_t Index = y * GlyphWidth + x;
            pGlyphBitMatrix[Index] = (pPixelGlyph[Index] & 0xF) == 0 ? false : true;
        }
    }

    auto newGlyph = Glyph{ GlyphWidth, GlyphHeight, pGlyphBitMatrix };
    delete[] pPixelGlyph;

    return newGlyph;
}
