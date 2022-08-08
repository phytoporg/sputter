#pragma once

#include <string>
#include <memory>

#include <sputter/game/object.h>
#include <sputter/math/fixedpoint.h>
#include <sputter/math/fpvector3d.h>

namespace sputter
{
    namespace render
    {
        class Mesh;
        class Shader;
        class MeshSubsystem;
    }
}

class CharacterCube : sputter::game::Object
{
public:
    CharacterCube(
        sputter::assets::AssetStorageProvider* pStorageProvider,
        sputter::game::SubsystemProvider* pSubsystemProvider
        );

    virtual void Tick(sputter::math::FixedPoint deltaTime) override;

    void Initialize(
        sputter::math::FixedPoint cubeSize,
        sputter::math::FPVector3D location
        );

private:
    static const std::string       kCubeVertexShaderAssetName;
    static const std::string       kCubeFragmentShaderAssetName;

    sputter::render::Mesh*                   m_pMeshComponent;
    std::shared_ptr<sputter::render::Shader> m_spShader;
};