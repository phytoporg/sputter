#include "fixedpoint.h"
#include <cstdio>

using namespace sputter;
using namespace sputter::math;

void ToString(const FixedPoint& fixedPoint, char* pBuffer)
{
    static_assert(FixedPoint::FRACTION_MULT == 1 << 8, "Unexpected fixed point format");

    const uint32_t RawValue = fixedPoint.raw_value();
    const int32_t IntPart = RawValue >> 8 & 0x00FFFFFF;
    const uint32_t FracPart = RawValue & 0xFF;

    sprintf(pBuffer, "%d.%u", IntPart, FracPart);
}
