#pragma once

// Generic Game object. Each game should inherit from this.

#include <sputter/math/fixedpoint.h>

namespace sputter { namespace game {
    class Game
    {
    public:
        virtual ~Game() {};

        virtual void Tick(math::FixedPoint deltaTime) = 0;
        virtual void PostTick(math::FixedPoint deltaTime) {}
        virtual void Draw() = 0;

        // Modeled after GGPO
        virtual bool StartGame() = 0;
        virtual bool SaveGameState(
            void** pBuffer,
            size_t* pSize,
            size_t* pChecksum,
            int frame) = 0;
        virtual bool LoadGameState(void** pBuffer, size_t size) = 0;
        virtual bool AdvanceFrame() = 0;
    };
}}
