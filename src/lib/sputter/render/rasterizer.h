#pragma once

#include <cstdint>

namespace sputter { namespace render {
    void DrawVerticalLine(int16_t x, int16_t y0, int16_t y1, uint8_t* pData, int16_t stride);
    void DrawHorizontalLine(int16_t y, int16_t x0, int16_t x1, uint8_t* pData, int16_t stride);
    void DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t* pData, int16_t stride);
}}