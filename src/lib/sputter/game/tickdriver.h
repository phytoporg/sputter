#pragma once

#include <cstdint>
#include <sputter/math/fixedpoint.h>

namespace sputter { namespace game {
    class ITickDriver
    {
    public:
        virtual void Initialize() = 0;
        virtual void Tick(math::FixedPoint dt) = 0;

    protected:
        virtual void TickOneFrame(math::FixedPoint dt, uint32_t p1InputMask, uint32_t p2InputMask) = 0;
    };
}}