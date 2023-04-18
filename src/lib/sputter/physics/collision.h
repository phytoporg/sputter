#pragma once

#include <vector>
#include <cstdint>

#include <sputter/game/objectstorage.h>
#include <sputter/game/subsystemtype.h>

#include <sputter/core/serializable.h>

#include <sputter/physics/aabb.h>

#include "collisionresult.h"

namespace sputter { namespace game {
    class Object;
}}

namespace sputter { namespace physics {
    struct CollisionResult;

    struct Collision : public core::ISerializable
    {
        static const game::SubsystemType ComponentId = game::SubsystemType::TYPE_COLLISION;

        // Begin ISerializable
        virtual bool Serialize(void* pBuffer, size_t* pBytesWrittenOut, size_t maxBytes) override;
        virtual bool Deserialize(void* pBuffer, size_t* pBytesReadOut, size_t maxBytes) override;
        virtual const char * GetName() const override { return "Collision"; };
        // End ISerializable

        bool Intersects(const Collision& other) const;
        bool TestIntersection(const Collision& other, CollisionResult* pCollisionResultOut) const;

        // Yet unused
        struct InitializationParameters {};

        game::objectstorage::ObjectHandle ObjectHandle = game::objectstorage::kInvalidHandle;

        static const size_t kMaxCollisionShapes = 8;
        AABB            CollisionShapes[kMaxCollisionShapes];
        size_t          NumCollisionShapes = 0;

        // Game-specified bitfields. Only objects with the same bits set can collide.
        uint32_t CollisionFlags = 0;
    };
}}

void ToString(const sputter::physics::Collision& collision, char* pBuffer);