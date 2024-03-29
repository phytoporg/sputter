#pragma once

#include <sputter/math/fixedpoint.h>

namespace sputter { namespace game {
    class IScene
    {
    public:
        virtual ~IScene() {}

        virtual void Initialize() = 0;
        virtual void Uninitialize() = 0;

        virtual void Tick(math::FixedPoint dt) = 0;
        virtual void Draw() = 0;
    };
} }