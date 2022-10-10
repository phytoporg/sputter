#pragma once

#include <sputter/math/fixedpoint.h>

// A Subsystem object in Sputter manages the lifetime of a particular kind of
// component related to the subsystem and is responsible for updating those
// components on each tick.

namespace sputter { namespace core {
    template<typename ComponentType>
    class ISubsystem
    {
    public:
        using Component = ComponentType;

        virtual void Tick(math::FixedPoint dt) {}
        virtual void PostTick(math::FixedPoint dt) {}

        virtual ComponentType* CreateComponent(const typename ComponentType::InitializationParameters& params) = 0;
        virtual void ReleaseComponent(ComponentType* pComponent) = 0;
    };
}}
