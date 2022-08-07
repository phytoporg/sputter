#pragma once

#include <string>

#include <sputter/game/object.h>
#include <sputter/math/fpvector2d.h>

// TODO: Refactor
#include <sputter/render/texture.h>

// Forward declarations
namespace sputter 
{ 
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
    TestObject(
        sputter::assets::AssetStorageProvider* pStorageProvider,
        sputter::game::SubsystemProvider* pSubsystemProvider
        );

    virtual void Tick(sputter::math::FixedPoint deltaTime) override;

    void Initialize(
        const sputter::math::FPVector2D& initialPosition
        );

private:
    sputter::physics::RigidBody2D*   m_pRigidBodyComponent;
    sputter::render::Sprite*         m_pSpriteComponent;

    static const std::string         kShipImageAssetName;
    static const std::string         kShipTextureName;
};
