#pragma once

#include <cstdint>

namespace sputter { namespace render {

    const uint8_t kRasterFlagSegmentHasDeltaX    = 1 << 0;
    const uint8_t kRasterFlagDeltaXIsNegative    = 1 << 1;
    const uint8_t kRasterFlagSegmentHasDeltaY    = 1 << 2;
    const uint8_t kRasterFlagDeltaYIsNegative    = 1 << 3;

    uint8_t ComputeWindingOrder(int16_t* pXCoordinates, int16_t* pYCoordinates, uint16_t numPoints);

    uint8_t SegmentToRasterFlags(int16_t x0, int16_t y0, int16_t x1, int16_t y1);
    void RasterFlagsToDeltas(uint8_t flags, int8_t* pDx, int8_t* pDy);

    void DrawVerticalLine(int16_t x, int16_t y0, int16_t y1, uint8_t* pData, uint16_t stride, uint8_t color);
    void DrawHorizontalLine(int16_t y, int16_t x0, int16_t x1, uint8_t* pData, uint16_t stride, uint8_t color);
    void DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t* pData, uint16_t stride, uint8_t color);

    void ScanlineFill(uint8_t* pScanline, uint16_t stride, uint8_t color);
}}