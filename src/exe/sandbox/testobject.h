#pragma once

#include <sputter/game/object.h>
#include <sputter/math/fpvector2d.h>

// TODO: Refactor
#include <sputter/render/texture.h>

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
        class TextureStorage;
    }
}

class TestObject : sputter::game::Object
{
public:
    TestObject(sputter::game::SubsystemProvider* pProvider);

    virtual void Tick(sputter::math::FixedPoint deltaTime) override;

    void Initialize(
        const sputter::math::FPVector2D& initialPosition,
        sputter::render::TexturePtr spTexture
        );

private:
    sputter::physics::RigidBody2D*   m_pRigidBodyComponent;
    sputter::render::Sprite*         m_pSpriteComponent;
};
