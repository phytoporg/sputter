#include "charactercube.h"

#include <sputter/assets/assetstorageprovider.h>
#include <sputter/math/fpconstants.h>
#include <sputter/render/meshsubsystem.h>
#include <sputter/render/shaderstorage.h>

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

    auto pShaderStorage = pStorageProvider->GetStorageByType<ShaderStorage>();
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
    using namespace sputter::math;
    // Place cube's lower-left corner at the local origin for now. Need working FP transforms
    // for less error-prone adjustments in placing the origin at the cube's actual center.
    static const FPVector3D VertexPositions[] = {
        // Bottom face
        FPVector3D(FPZero, FPZero, FPZero),
        FPVector3D(cubeSize, FPZero, FPZero),
        FPVector3D(cubeSize, cubeSize, FPZero),
        FPVector3D(FPZero, cubeSize, FPZero), 

        // Top face
        FPVector3D(FPZero, FPZero, cubeSize),
        FPVector3D(cubeSize, FPZero, cubeSize),
        FPVector3D(cubeSize, cubeSize, cubeSize),
        FPVector3D(FPZero, cubeSize, cubeSize), 
    };

    static const FixedPoint FPHalfSize = cubeSize / FPTwo;
    static const FPVector3D VertexNormals[] = {
        // Bottom face
        FPVector3D(-FPHalfSize, -FPHalfSize, -FPHalfSize).Normalized(),
        FPVector3D( FPHalfSize, -FPHalfSize, -FPHalfSize).Normalized(),
        FPVector3D( FPHalfSize, -FPHalfSize,  FPHalfSize).Normalized(),
        FPVector3D(-FPHalfSize, -FPHalfSize,  FPHalfSize).Normalized(), 

        // Top face
        FPVector3D(-FPHalfSize, FPHalfSize, -FPHalfSize).Normalized(),
        FPVector3D( FPHalfSize, FPHalfSize, -FPHalfSize).Normalized(),
        FPVector3D( FPHalfSize, FPHalfSize,  FPHalfSize).Normalized(),
        FPVector3D(-FPHalfSize, FPHalfSize,  FPHalfSize).Normalized(), 
    };

    // TODO: worry about this when we actually want a texture
    static const FPVector2D VertexUVs[] = {
        // Bottom face
        FPVector2D(FPZero, FPZero),
        FPVector2D(FPZero, FPZero),
        FPVector2D(FPZero, FPZero),
        FPVector2D(FPZero, FPZero),

        // Top face
        FPVector2D(FPZero, FPZero),
        FPVector2D(FPZero, FPZero),
        FPVector2D(FPZero, FPZero),
        FPVector2D(FPZero, FPZero),
    };

    // Bottom face:
    // 3 ***** 2  |
    // *       *  | Front
    // *       *  |
    // *       *  V
    // 0 ***** 1
    //
    // Top face:
    // 7 ***** 6
    // *       *
    // *       *
    // *       *
    // 4 ***** 5

    // Assuming GL_CCW for front-facing
    static const int VertexIndices[] = {
        // Bottom face
        2, 1, 0, 2, 0, 3,
        // Front face
        5, 0, 1, 4, 0, 5,
        // Left face
        4, 3, 0, 7, 3, 4,
        // Right face
        5, 1, 2, 6, 2, 5,
        // Back face
        6, 2, 3, 7, 6, 3,
        // Top face
        6, 5, 4, 7, 6, 4,
    };

    const uint32_t NumVertices = sizeof(VertexPositions) / sizeof(VertexPositions[0]); 
    m_pMeshComponent->SetPositions(VertexPositions, NumVertices);
    m_pMeshComponent->SetNormals(VertexNormals, NumVertices);
    m_pMeshComponent->SetTextureCoordinates(VertexUVs, NumVertices);
    m_pMeshComponent->SetShader(nullptr/*TODO*/);
}