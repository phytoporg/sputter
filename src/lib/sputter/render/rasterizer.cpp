#include "rasterizer.h"

void sputter::render::DrawVerticalLine(uint8_t x, uint8_t y0, uint8_t y1, uint8_t* pData, uint8_t stride) 
{
    const int8_t SignedDifference = (int8_t)y1 - (int8_t)y0;
    const uint8_t NumPixels = SignedDifference > 0 ? (y1 - y0) : (y0 - y1);
    const int8_t Step = SignedDifference < 0 ? -1 : 1;
    for (int i = 0; i < NumPixels; ++i)
    {
        pData[x + (y0 + i * Step) * stride] = 1;
    }
}

void sputter::render::DrawHorizontalLine(uint8_t y, uint8_t x0, uint8_t x1, uint8_t* pData, uint8_t stride) 
{
    const int8_t SignedDifference = (int8_t)x1 - (int8_t)x0;
    const uint8_t NumPixels = SignedDifference > 0 ? (x1 - x0) : (x0 - x1);
    const int8_t Step = SignedDifference < 0 ? -1 : 1;
    for (int i = 0; i < NumPixels; ++i)
    {
        pData[(x0 + Step) + (y * stride)] = 1;
    }
}
