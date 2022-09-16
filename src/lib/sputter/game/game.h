#pragma once

// Generic Game object. Each game should inherit from this.

#include <sputter/math/fixedpoint.h>

namespace sputter { namespace game {
    class Game
    {
    public:
        virtual ~Game() {};

        virtual void Tick(math::FixedPoint deltaTime) = 0;
        virtual void Draw() = 0;
        virtual bool StartGame() = 0;
    };
}}
