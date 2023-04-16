#pragma once

#include <vector>
#include <sputter/core/component.h>
#include <sputter/core/serializable.h>
#include <sputter/core/subsystem.h>

#include <sputter/physics/collision.h>

#include <sputter/containers/arrayvector.h>

#include "collisionresult.h"

namespace sputter { namespace physics {
    class ICollisionShape;
    struct CollisionSubsystemSettings
    {
        // TODO?
    };

    class CollisionSubsystem : public core::ISubsystem<Collision>, public core::ISerializable
    {
    public:
        static const game::SubsystemType SubsystemId = game::SubsystemType::TYPE_COLLISION;

        CollisionSubsystem(const CollisionSubsystemSettings& settings);

        // Begin ISubsystem
        virtual void PostTick(math::FixedPoint dt) override;
        virtual Collision* CreateComponent(const Collision::InitializationParameters& params) override;
        virtual void ReleaseComponent(Collision* pComponent) override;
        // End ISubsystem

        // Begin ISerializable
        virtual bool Serialize(void* pBuffer, size_t* pBytesWrittenOut, size_t maxBytes) override;
        virtual bool Deserialize(void* pBuffer, size_t* pBytesReadOut, size_t maxBytes) override;
        virtual const char * GetName() const override { return "CollisionSubsystem"; };
        // End ISerializable

        core::ComponentHandle GetComponentHandle(Collision* pCollision) const;
        Collision* GetComponentFromHandle(core::ComponentHandle handle);

        const static size_t kMaxCollisionResults = 16;
        bool GetCollisionResultsForThisFrame(
            Collision* pCollision,
            containers::ArrayVector<CollisionResult, kMaxCollisionResults>* pResultsOut);
        bool GetCollisionResultsForThisFrame(
            core::ComponentHandle collisionHandle,
            containers::ArrayVector<CollisionResult, kMaxCollisionResults>* pResultsOut);

    private:
        // No default or copy construction allowed here
        CollisionSubsystem() = delete;
        CollisionSubsystem(const CollisionSubsystem& other) = delete;

        static const size_t kMaxCollisions = 16;
        Collision m_collisions[kMaxCollisions] = {};
        size_t    m_collisionCount = 0;

        CollisionResult CollisionsThisFrame[kMaxCollisionResults];
        size_t          NumCollisionsThisFrame = 0;
    };
}}