#pragma once

#include <sputter/game/object.h>

// Forward declarations
namespace sputter 
{ 
    namespace game 
    {
        class SubsystemProvider;
    }

    namespace physics 
    {
        class RigidBody2D;
    }

    namespace render 
    {
        class Sprite;
    }
}

class TestObject : sputter::game::Object
{
public:
    TestObject(sputter::game::SubsystemProvider* pProvider);

    virtual void Tick(float dt) override;

private:
    sputter::physics::RigidBody2D*   m_pRigidBodyComponent;
    sputter::render::Sprite*         m_pSpriteComponent;
};
