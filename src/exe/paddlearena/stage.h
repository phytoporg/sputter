#pragma once

#include <string>
#include <memory>
#include <sputter/game/object.h>
#include <sputter/math/fpvector2d.h>
#include <sputter/math/fptransform3d.h>

namespace sputter {
    namespace render {
        class Mesh;
        class Shader;
    }

    namespace physics {
        class Collision;
    }
}

class Stage : sputter::game::Object
{
public:
    Stage(
        sputter::assets::AssetStorageProvider* pStorageProvider,
        sputter::game::SubsystemProvider* pSubsystemProvider
    );

    virtual void Tick(sputter::math::FixedPoint deltaTime) override;

    void Initialize(
        sputter::math::FPVector2D stageDimensions
        );

private:
    static const std::string       kArenaVertexShaderAssetName;
    static const std::string       kArenaFragmentShaderAssetName;
    static const std::string       kArenaShaderName;

    sputter::render::Mesh*                   m_pMeshComponent = nullptr;
    sputter::physics::Collision*             m_pCollisionComponent = nullptr;
    std::shared_ptr<sputter::render::Shader> m_spShader;

    sputter::math::FPTransform3D             m_localTransform;
    sputter::math::FPVector3D                m_initialLocation;
};