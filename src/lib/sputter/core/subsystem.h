#pragma once

// A Subsystem object in Sputter manages the lifetime of a particular kind of
// component related to the subsystem and is responsible for updating those
// components on each tick.

namespace sputter { namespace core {
    template<typename ComponentType>
    class ISubsystem
    {
    public:
        virtual void Tick(float dt) = 0;

        virtual ComponentType* CreateComponent() = 0;
        virtual void ReleaseComponent(ComponentType* pComponent) = 0;
    };
}}
