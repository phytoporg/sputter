#pragma once

#include <vector>
#include <memory>
#include "rigidbody2d.h"

#include <sputter/core/subsystem.h>
// TODO: This should probably be in 'core'?
#include <sputter/game/subsystemtype.h>
#include <sputter/math/fpvector2d.h>
#include <sputter/math/fpconstants.h>

namespace sputter { namespace physics {
    struct RigidBodySubsystemSettings
    {
        size_t           MaxRigidBodies   = 10;
        bool             IsGravityEnabled = true;
        math::FPVector2D Gravity          = math::FPVector2D(math::FPZero, -math::FPTen);
    };

    class RigidBodySubsystem : public core::ISubsystem<RigidBody2D>
    {
    public:
        static const game::SubsystemType SubsystemId = game::SubsystemType::TYPE_RIGIDBODY;

        RigidBodySubsystem(const RigidBodySubsystemSettings& settings);

        // Begin ISubsystem
        virtual void Tick(math::FixedPoint dt) override;
        virtual RigidBody2D* CreateComponent(const RigidBody2D::InitializationParameters& params) override;
        virtual void ReleaseComponent(RigidBody2D* pComponent) override;
        // End ISubsystem

    private:
        // No default or copy construction allowed here
        RigidBodySubsystem() = delete;
        RigidBodySubsystem(const RigidBodySubsystem& other) = delete;

        std::vector<RigidBody2D> m_rigidBodies;
        std::vector<bool>        m_validRigidBodyVector;

        bool                     m_isGravityEnabled;
        math::FPVector2D         m_gravity;
        size_t                   m_maxRigidBodies;
        size_t                   m_rigidBodyCount;
    };
}}
