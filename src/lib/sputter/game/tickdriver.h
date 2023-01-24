#pragma once

#include <cstdint>
#include <sputter/math/fixedpoint.h>

namespace sputter { namespace game {
    class ITickDriver
    {
    public:
        virtual void Tick(math::FixedPoint dt) = 0;

    protected:
        virtual void TickFrames(uint32_t start, uint32_t end, math::FixedPoint dt) = 0;
        virtual void TickOneFrame(math::FixedPoint dt) = 0;
    };
}}