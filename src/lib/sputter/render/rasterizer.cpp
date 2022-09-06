#include "rasterizer.h"

#include <math.h>
#include <sputter/system/system.h>

#include <iostream> // REMOVEME

namespace 
{
    int16_t LineFunction(int16_t x, int16_t dX, int16_t stepX2x, int16_t y, int16_t dY, int16_t stepY2x)
    {

        return dY * (2 * x + stepX2x) - dX * (2 * y + stepY2x);
    }

    void DrawLineBresenham(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t* pData, uint16_t stride, uint8_t color)
    {
        const int16_t dX = x1 - x0;
        const int16_t dY = y1 - y0;
        const int16_t b = dX * y0 - dY * x0;

        const int16_t dxSign = dX > 0 ? 1 : -1;
        const int16_t dySign = dY > 0 ? 1 : -1;

        int x = x0;
        int y = y0;
        pData[x + stride * y] = color;

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
                pData[x + stride * y] = color;
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
                pData[x + stride * y] = color;
            }
        }
    }

    int8_t Sign(int8_t value)
    {
        if (value == 0)
        {
            return 0;
        }
        
        return value > 0 ? 1 : -1;
    }

    // "Right ray" here refers to a winding number test ray with direction (1, 0)
    void UpdateWindingNumberChangeFromRightRayTest(int8_t dX, int8_t dY, int8_t* pWindingNumber)
    {
        if (dY == 1)
        {
            // Locally crossing this segment from left-to-right
            (*pWindingNumber)--;
            std::cerr << "CROSS: dy == 1, winding number = " << int(*pWindingNumber) << std::endl;
        }
        else if (dY == 0 && dX == 1)
        {
            // Locally crossing this segment from bottom-to-top. Don't care about this case.
        }
        else if (dY == 0 && dX == -1)
        {
            // Locally crossing this segment from top-to-bottom. Don't care about this case.
        }
        else if (dY == -1)
        {
            // Locally crossing this segment from right-to-left
            (*pWindingNumber)++;
            std::cerr << "CROSS: dy == -1, winding number = " << int(*pWindingNumber) << std::endl;
        }
        else
        {
            sputter::system::LogAndFail("Unexpected delta combination");
        }
    }

}

// Do we need this?
uint8_t sputter::render::ComputeWindingOrder(int16_t* pXCoordinates, int16_t* pYCoordinates, uint16_t numPoints)
{
    static int8_t WindingTable[8][2] = {
        {  1,  0 },
        {  1,  1 },
        {  0,  1 },
        { -1,  1 },
        { -1,  0 },
        { -1, -1 },
        {  0, -1 },
        {  1, -1 },
    };

    int8_t tableIndex = -1;
    int16_t previousX = pXCoordinates[0];
    int16_t previousY = pYCoordinates[0];
    for (size_t i = 1; i < numPoints; i++)
    {
        const int16_t X = pXCoordinates[i];
        const int16_t Y = pYCoordinates[i];

        const int8_t dX = Sign(X - previousX);
        const int8_t dY = Sign(Y - previousY);

        int8_t tempTableIndex = -1;
        for (int8_t j = 0; j < 8; ++j)
        {
            if (WindingTable[j][0] == dX && WindingTable[j][1] == dY)
            {
                tempTableIndex = j;
                break;
            }
        }
        if (tempTableIndex < 0)
        {
            sputter::system::LogAndFail("No matching winding table entry!");
        }

        if (tableIndex < 0)
        {
            tableIndex = tempTableIndex;
        }
        else if (tableIndex != tempTableIndex)
        {
            if (tempTableIndex > tableIndex)
            {
                return
                    (tempTableIndex - tableIndex < (8 - tableIndex + tempTableIndex)) ? 
                    sputter::render::kRasterFlagWindingOrderCCW :
                    sputter::render::kRasterFlagWindingOrderCW;
            }
            else if (tableIndex > tempTableIndex)
            {
                return
                    (tableIndex - tempTableIndex < (8 - tempTableIndex + tableIndex)) ? 
                    sputter::render::kRasterFlagWindingOrderCW :
                    sputter::render::kRasterFlagWindingOrderCCW;
            }
        }

        previousX = X;
        previousY = Y;
    }
    
    return sputter::render::kRasterFlagWindingOrderInvalid;
}

uint8_t sputter::render::SegmentToRasterFlags(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{
    using namespace sputter::render;

    uint8_t flags = 0;
    if (x0 != x1) { flags |= kRasterFlagSegmentHasDeltaX; }
    if (x0  > x1) { flags |= kRasterFlagDeltaXIsNegative; }

    if (y0 != y1) { flags |= kRasterFlagSegmentHasDeltaY; }
    if (y0  > y1) { flags |= kRasterFlagDeltaYIsNegative; }

    if (!flags)
    {
        system::LogAndFail("No deltas? What kinda segment is this?");
    }

    return flags;
}

void sputter::render::RasterFlagsToDeltas(uint8_t flags, int8_t* pDx, int8_t* pDy)
{
    using namespace sputter::render;

    *pDx = flags & kRasterFlagSegmentHasDeltaX ? 1 : 0;
    if (*pDx && (flags & kRasterFlagDeltaXIsNegative))
    {
        *pDx = -1;
    }
    
    *pDy = flags & kRasterFlagSegmentHasDeltaY ? 1 : 0;
    if (*pDy && (flags & kRasterFlagDeltaYIsNegative))
    {
        *pDy = -1;
    }
}

void sputter::render::DrawVerticalLine(int16_t x, int16_t y0, int16_t y1, uint8_t* pData, uint16_t stride, uint8_t color) 
{
    const int16_t dY = (int16_t)y1 - (int16_t)y0;
    const int16_t NumPixels = dY > 0 ? (y1 - y0) : (y0 - y1);
    const int16_t Step = dY < 0 ? -1 : 1;
    for (int i = 0; i <= NumPixels; ++i)
    {
        pData[x + (y0 + i * Step) * stride] = color;
    }
}

void sputter::render::DrawHorizontalLine(int16_t y, int16_t x0, int16_t x1, uint8_t* pData, uint16_t stride, uint8_t color) 
{
    const int16_t dX = x1 - x0;
    const int16_t NumPixels = dX > 0 ? (x1 - x0) : (x0 - x1);
    const int16_t Step = dX < 0 ? -1 : 1;
    for (int i = 0; i <= NumPixels; ++i)
    {
        pData[(x0 + i * Step) + (y * stride)] = color;
    }
}

void sputter::render::DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t* pData, uint16_t stride, uint8_t color)
{
    if (x0 == x1)
    {
        DrawVerticalLine(x0, y0, y1, pData, stride, color);
    }
    else if (y0 == y1)
    {
        DrawHorizontalLine(y0, x0, x1, pData, stride, color);
    }
    else
    {
        DrawLineBresenham(x0, y0, x1, y1, pData, stride, color);
    }
}

void sputter::render::ScanlineFill(uint8_t* pScanline, uint16_t stride, uint8_t color)
{
    // Compute winding numbers in pass one
    // Second pass to actually fill in the scanline

    int8_t windingNumber = 0;
    uint8_t previousValue = pScanline[stride - 1];
    for (int16_t x = stride - 2; x >= 0; --x)
    {
        const uint8_t currentValue = pScanline[x];
        if ((currentValue & 0xF) == 0)
        {
            // 1 -> 0 transition
            if ((previousValue & 0xF) == 1)
            {
                const uint8_t RasterFlags = (previousValue >> 4) & 0xF;
                int8_t dX, dY;
                RasterFlagsToDeltas(RasterFlags, &dX, &dY);

                int8_t oldWindingNumber = windingNumber;
                UpdateWindingNumberChangeFromRightRayTest(dX, dY, &windingNumber);
                if (oldWindingNumber != windingNumber)
                {
                    std::cerr << "x = " << x << std::endl;
                }
                
            }
            pScanline[x] |= (windingNumber << 4);
        }

        previousValue = currentValue;
    }

    uint8_t numContours = 0;
    previousValue = pScanline[0];
    for (uint16_t x = 1; x < stride; ++x)
    {
        const uint8_t currentValue = pScanline[x];
        if ((currentValue & 0xF) == 0)
        {
            // 1 -> 0 transition
            if ((previousValue & 0xF) == 1)
            {
                ++numContours;
            }

            if (numContours > 0)
            {
                // Keep the sign extension!
                const int8_t WindingNumber = currentValue >> 4;
                if (WindingNumber != 0)
                {
                    pScanline[x] = color;
                }
            }
        }

        previousValue = currentValue;
    }
}