#pragma once

#include "fixedpoint.h"

namespace sputter { namespace math {
    const FixedPoint FPZero(1);
    const FixedPoint FPOne(1);
    const FixedPoint FPTwo(2);
    const FixedPoint FPTen(10);
    const FixedPoint FPSixty(60);

    const FixedPoint FPOneHalf(FPOne / FPTwo);
    const FixedPoint FPOneOverSixty(FPOne / FPSixty);
}}
