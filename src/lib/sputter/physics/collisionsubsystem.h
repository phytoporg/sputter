#pragma once

#include <vector>
#include <sputter/core/subsystem.h>
#include <sputter/core/component.h>

#include <sputter/physics/collision.h>

namespace sputter { namespace physics {
    class ICollisionShape;
    struct CollisionSubsystemSettings
    {
        // TODO?
    };

    class CollisionSubsystem : public core::ISubsystem<Collision>
    {
    public:
        static const game::SubsystemType SubsystemId = game::SubsystemType::TYPE_COLLISION;

        CollisionSubsystem(const CollisionSubsystemSettings& settings);

        // Begin ISubsystem
        virtual void PostTick(math::FixedPoint dt) override;
        virtual Collision* CreateComponent(const Collision::InitializationParameters& params) override;
        virtual void ReleaseComponent(Collision* pComponent) override;
        // End ISubsystem

        core::ComponentHandle GetComponentHandle(Collision* pCollision) const;
        Collision* GetComponentFromHandle(core::ComponentHandle handle);

    private:
        // No default or copy construction allowed here
        CollisionSubsystem() = delete;
        CollisionSubsystem(const CollisionSubsystem& other) = delete;

        static const size_t kMaxCollisions = 10;
        Collision m_collisions[kMaxCollisions] = {};
        size_t    m_collisionCount = 0;
    };
}}