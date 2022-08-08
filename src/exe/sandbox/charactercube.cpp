#include "charactercube.h"

#include <sputter/render/meshsubsystem.h>

using namespace sputter::render;
using namespace sputter::game;
using namespace sputter::assets;
using namespace sputter::math;

const std::string CharacterCube::kCubeVertexShaderAssetName = "cube_vert";
const std::string CharacterCube::kCubeFragmentShaderAssetName = "cube_frag";

CharacterCube::CharacterCube(
    AssetStorageProvider* pStorageProvider,
    SubsystemProvider* pSubsystemProvider
) : Object(pStorageProvider, pSubsystemProvider)
{
    CreateAndSetComponentByType<MeshSubsystem>(&m_pMeshComponent);
    if (!m_pMeshComponent)
    {
        sputter::system::LogAndFail("Failed to create mesh component in CharacterCube object.");
    }
}

void CharacterCube::Tick(FixedPoint deltaTime)
{
    // TODO
}

void CharacterCube::Initialize(
    sputter::math::FixedPoint cubeSize,
    sputter::math::FPVector3D location
    )
{
    // TODO: Generate geometry + indices
}