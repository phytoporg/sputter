#pragma once

#include <string>
#include <memory>

#include <sputter/game/object.h>
#include <sputter/math/fixedpoint.h>
#include <sputter/math/fptransform3d.h>
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
    static const std::string       kCubeShaderName;

    sputter::render::Mesh*                   m_pMeshComponent;
    std::shared_ptr<sputter::render::Shader> m_spShader;

    sputter::input::InputSource*             m_pInputSource;

    sputter::math::FPTransform3D             m_localTransform;
    sputter::math::FixedPoint                m_accumulatedTime;
    sputter::math::FPVector3D                m_initialLocation;
};