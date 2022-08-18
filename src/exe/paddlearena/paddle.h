#pragma once

#include <string>
#include <memory>

#include <sputter/game/object.h>
#include <sputter/math/fixedpoint.h>
#include <sputter/math/fptransform3d.h>
#include <sputter/math/fpvector2d.h>
#include <sputter/math/fpvector3d.h>

namespace sputter
{
    namespace render
    {
        class Mesh;
        class Shader;
        class MeshSubsystem;
    }

    namespace input
    {
        class InputSource;
    }

    namespace physics
    {
        class Collision;
    }
}

class Paddle : sputter::game::Object
{
public:
    Paddle(
        uint32_t playerId,
        sputter::assets::AssetStorageProvider* pStorageProvider,
        sputter::game::SubsystemProvider* pSubsystemProvider
        );

    virtual void Tick(sputter::math::FixedPoint deltaTime) override;

    void Initialize(
        sputter::math::FPVector2D dimensions,
        sputter::math::FPVector3D location
        );

    virtual void HandleCollision(Object const* pOther) override;

private:
    static const std::string       kPaddleVertexShaderAssetName;
    static const std::string       kPaddleFragmentShaderAssetName;
    static const std::string       kPaddleShaderName;

    sputter::render::Mesh*                   m_pMeshComponent = nullptr;
    std::shared_ptr<sputter::render::Shader> m_spShader = nullptr;
    sputter::physics::Collision*             m_pCollisionComponent = nullptr;

    sputter::input::InputSource*             m_pInputSource = nullptr;

    sputter::math::FPTransform3D             m_localTransform;
    sputter::math::FPVector3D                m_initialLocation;
};