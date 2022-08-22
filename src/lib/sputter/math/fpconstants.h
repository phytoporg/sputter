#pragma once

#include "fixedpoint.h"
#include "fpvector3d.h"

namespace sputter { namespace math {
    const FixedPoint FPZero(0);
    const FixedPoint FPOne(1);
    const FixedPoint FPTwo(2);
    const FixedPoint FPTen(10);
    const FixedPoint FPTwenty(20);
    const FixedPoint FPThirty(30);
    const FixedPoint FPSixty(60);
    const FixedPoint FPOneThousand(1000);

    const FixedPoint FPOneHalf(FPOne / FPTwo);
    const FixedPoint FPOneOverSixty(FPOne / FPSixty);

    const FixedPoint FPEpsilon(FPOne / FPOneThousand);
}}
