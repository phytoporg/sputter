#include "rasterizer.h"

#include <math.h>

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

void sputter::render::DrawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t* pData, uint8_t stride)
{
    const int8_t dX = x1 - x0;
    const int8_t dY = y1 - y0;
    const int8_t b = dX * y0 - dY * x0;

    int x = x0;
    int y = y0;
    pData[x + stride * y] = 1;

    const int8_t dxSign = dX > 0 ? 1 : -1;
    const int8_t dySign = dY > 0 ? 1 : -1;

    const int8_t A = 2 * dY;
    const int8_t B = -2 * dX;
    const int8_t C = 2 * b;

    // Shallow 
    if (abs(dX) > abs(dY))
    {
        const int8_t stepX2x = 2 * dxSign;
        const int8_t stepY2x = dySign;

        while(x != x1)
        {
            const int8_t f = dY * (2 * x + stepX2x) - dX * (2 * y + stepY2x);
            if (dY > 0 && f > 0)
            {
                ++y;
            }
            else if (dY < 0 && f < 0)
            {
                --y;
            }
            
            pData[x + stride * y] = 1;
            x += dxSign;
        }
    }
    // Steep
    else
    {
        const int8_t stepX2x = dxSign;
        const int8_t stepY2x = 2 * dySign;

        while(y != y1)
        {
            const int8_t f = dY * (2 * x + stepX2x) - dX * (2 * y + stepY2x);
            if (dX > 0 && f > 0)
            {
                ++x;
            }
            else if (dX < 0 && f < 0)
            {
                --x;
            }
            
            pData[x + stride * y] = 1;
            y += dySign;
        }
    }
}
