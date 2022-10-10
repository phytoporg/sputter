#pragma once

#include <vector>
#include <sputter/core/subsystem.h>
#include <sputter/core/component.h>

#include <sputter/physics/collision.h>

namespace sputter { namespace physics {
    class ICollisionShape;
    struct CollisionResult
    {
        CollisionResult() {}
        CollisionResult(
            const Collision* pA,
            const Collision* pB,
            const ICollisionShape* pShapeA,
            const ICollisionShape* pShapeB);

        const Collision* pCollisionA = nullptr;
        const Collision* pCollisionB = nullptr;
        const ICollisionShape* pCollisionShapeA = nullptr;
        const ICollisionShape* pCollisionShapeB = nullptr;
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

        std::vector<Collision>       m_collisions;
    };
}}