#pragma once

#include <fpm/fixed.hpp>

namespace sputter { namespace math {
    using FixedPoint = fpm::fixed_24_8;

}}

void ToString(const sputter::math::FixedPoint& fixedPoint, char* pBuffer);
