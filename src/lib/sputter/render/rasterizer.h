#pragma once

#include <cstdint>

namespace sputter { namespace render {
    void DrawVerticalLine(uint8_t x, uint8_t y0, uint8_t y1, uint8_t* pData, uint8_t stride);
    void DrawHorizontalLine(uint8_t y, uint8_t x0, uint8_t x1, uint8_t* pData, uint8_t stride);
    void DrawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t* pData, uint8_t stride);
}}