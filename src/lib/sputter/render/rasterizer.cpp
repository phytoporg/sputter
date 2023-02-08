#include "rasterizer.h"

#include <math.h>
#include <sputter/system/system.h>
#include <sputter/log/log.h>

namespace 
{
    int16_t LineFunction(int16_t x, int16_t dX, int16_t stepX2x, int16_t y, int16_t dY, int16_t stepY2x)
    {
        // 2 * f(x, y) = 0 derived from f(x, y) = dY * x - dX * y + b
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
            RELEASE_LOGLINE_VERYVERBOSE(LOG_FONT, "CROSS: dy == 1, winding number = %d", int(*pWindingNumber));
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
            RELEASE_LOGLINE_VERYVERBOSE(LOG_FONT, "CROSS: dy == -1, winding number = %d", int(*pWindingNumber));
        }
        else
        {
            sputter::system::LogAndFail("Unexpected delta combination");
        }
    }

    int8_t WindingNumberDeltaFromContourPixelValue(uint8_t pixelValue)
    {
        const uint8_t RasterFlags = (pixelValue >> 4) & 0xF;
        int8_t dX, dY;
        sputter::render::RasterFlagsToDeltas(RasterFlags, &dX, &dY);

        int8_t windingNumber = 0; 
        UpdateWindingNumberChangeFromRightRayTest(dX, dY, &windingNumber);
        return windingNumber;
    }
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

    // TODO: This can surely be done in a single pass?

    uint8_t previousValue = 0; // Init to zero, in case rightmost pixel is a contour pixel
    int8_t runningWindingNumber = 0;
    int8_t initialContourWindingNumber = 0;
    for (int16_t x = stride - 1; x >= 0; --x)
    {
        // There are three cases to handle in this loop:
        //
        // Case 1: A contour is one pixel wide on this scanline. The resulting winding number for this
        // contour is singular and can be computed directly from the encountered pixel.
        //
        // Case 2: A contour is multiple pixels on this scanline. The initially-encountered pixel and
        // the "exit" pixel for the countour run in the same vertical direction; the winding number
        // contribution is the same as if the counter were a single pixel wide in that direction.
        //
        // Case 3: Similar to case 2, multiple pixels encountered on the scanline but the initial and
        // exit pixels *differ* in vertical direction. In this case, the winding number contributions
        // cancel out, leaving runningWindingNumber unchanged.
        const uint8_t currentValue = pScanline[x];
        if ((currentValue & 0xF) == 1 && (previousValue & 0xF) == 0)
        {
            // Entering a contour from the right. Check the initial contour winding number.
            initialContourWindingNumber = WindingNumberDeltaFromContourPixelValue(currentValue);
        }
        else if ((currentValue & 0xF) == 0 && (previousValue & 0xF) == 1)
        {
            // Exiting a contour on its left edge.
            int8_t exitPixelWindingNumber = WindingNumberDeltaFromContourPixelValue(previousValue);
            if (exitPixelWindingNumber == initialContourWindingNumber)
            {
                runningWindingNumber += initialContourWindingNumber;
            }
        }

        if ((currentValue & 0xF) == 0)
        {
            pScanline[x] |= (runningWindingNumber << 4);
        }

        previousValue = currentValue;
    }

    for (uint16_t x = 0; x < stride; ++x)
    {
        const uint8_t currentValue = pScanline[x];
        if ((currentValue & 0xF) == 0)
        {
            // Keep the sign extension!
            const int8_t WindingNumber = currentValue >> 4;
            if (WindingNumber != 0)
            {
                pScanline[x] = color;
            }
        }
    }
}
