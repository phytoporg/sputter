#include "rasterizer.h"

#include <math.h>

namespace 
{
    int16_t LineFunction(int16_t x, int16_t dX, int16_t stepX2x, int16_t y, int16_t dY, int16_t stepY2x)
    {

        return dY * (2 * x + stepX2x) - dX * (2 * y + stepY2x);
    }

    void DrawLineBresenham(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t* pData, int16_t stride)
    {
        const int16_t dX = x1 - x0;
        const int16_t dY = y1 - y0;
        const int16_t b = dX * y0 - dY * x0;

        const int16_t dxSign = dX > 0 ? 1 : -1;
        const int16_t dySign = dY > 0 ? 1 : -1;

        int x = x0;
        int y = y0;
        pData[x + stride * y] = 1;

        // Shallow 
        if (abs(dX) >= abs(dY))
        {
            const int16_t stepX2x = 2 * dxSign;
            const int16_t stepY2x = dySign;

            while(x != x1)
            {
                const int16_t f = LineFunction(x, dX, stepX2x, y, dY, stepY2x);
                if (f > 0)
                {
                    y += dySign;
                }
                
                x += dxSign;
                pData[x + stride * y] = 1;
            }
        }
        // Steep
        else
        {
            const int16_t stepX2x = dxSign;
            const int16_t stepY2x = 2 * dySign;

            while(y != y1)
            {
                const int16_t f = LineFunction(x, dX, stepX2x, y, dY, stepY2x);
                if (f > 0)
                {
                    x += dxSign;
                }
                
                y += dySign;
                pData[x + stride * y] = 1;
            }
        }
    }

}

void sputter::render::DrawVerticalLine(int16_t x, int16_t y0, int16_t y1, uint8_t* pData, int16_t stride) 
{
    const int16_t dY = (int16_t)y1 - (int16_t)y0;
    const int16_t NumPixels = dY > 0 ? (y1 - y0) : (y0 - y1);
    const int16_t Step = dY < 0 ? -1 : 1;
    for (int i = 0; i <= NumPixels; ++i)
    {
        pData[x + (y0 + i * Step) * stride] = 1;
    }
}

void sputter::render::DrawHorizontalLine(int16_t y, int16_t x0, int16_t x1, uint8_t* pData, int16_t stride) 
{
    const int16_t dX = x1 - x0;
    const int16_t NumPixels = dX > 0 ? (x1 - x0) : (x0 - x1);
    const int16_t Step = dX < 0 ? -1 : 1;
    for (int i = 0; i <= NumPixels; ++i)
    {
        pData[(x0 + i * Step) + (y * stride)] = 1;
    }
}

void sputter::render::DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t* pData, int16_t stride)
{
    if (x0 == x1)
    {
        DrawVerticalLine(x0, y0, y1, pData, stride);
    }
    else if (y0 == y1)
    {
        DrawHorizontalLine(y0, x0, x1, pData, stride);
    }
    else
    {
        DrawLineBresenham(x0, y0, x1, y1, pData, stride);
    }
}
