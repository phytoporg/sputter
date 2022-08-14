#pragma once

#include <vector>
#include <sputter/core/subsystem.h>

#include <sputter/physics/collision.h>

namespace sputter { namespace physics {
    struct CollisionResult
    {
        Collision const* pCollisionA = nullptr;
        Collision const* pCollisionB = nullptr;
    };

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
        virtual void Tick(math::FixedPoint dt) override;
        virtual Collision* CreateComponent(const Collision::InitializationParameters& params) override;
        virtual void ReleaseComponent(Collision* pComponent) override;
        // End ISubsystem

        void ProcessCollisionResults();

    private:
        // No default or copy construction allowed here
        CollisionSubsystem() = delete;
        CollisionSubsystem(const CollisionSubsystem& other) = delete;

        std::vector<Collision>       m_collisions;
        std::vector<CollisionResult> m_collisionResults;
    };
}}