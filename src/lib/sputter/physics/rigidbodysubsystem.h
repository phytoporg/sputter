#pragma once

#include <vector>
#include <memory>
#include "rigidbody2d.h"

#include <sputter/core/subsystem.h>
#include <sputter/math/vector2d.h>

namespace sputter { namespace physics {
    struct RigidBodySubsystemSettings
    {
        size_t         MaxRigidBodies   = 10;
        bool           IsGravityEnabled = true;
        math::Vector2D Gravity          = math::Vector2D(0.f, -9.81f);
    };

    class RigidBodySubsystem : public core::ISubsystem<RigidBody2D>
    {
    public:
        RigidBodySubsystem(const RigidBodySubsystemSettings& settings);

        // Begin ISubsystem
        virtual void Tick(float dt) override;
        virtual RigidBody2D* CreateComponent() override;
        virtual void ReleaseComponent(RigidBody2D* pComponent) override;
        // End ISubsystem

    private:
        // No default or copy construction allowed here
        RigidBodySubsystem() = delete;
        RigidBodySubsystem(const RigidBodySubsystem& other) = delete;

        std::vector<RigidBody2D> m_rigidBodies;
        std::vector<bool>        m_validRigidBodyVector;

        bool                     m_isGravityEnabled;
        math::Vector2D           m_gravity;
        size_t                   m_maxRigidBodies;
        size_t                   m_rigidBodyCount;
    };
}}
