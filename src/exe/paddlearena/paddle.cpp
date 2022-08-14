#include "paddle.h"
#include "paddlearena.h"

#include <sputter/assets/assetstorageprovider.h>

#include <sputter/math/fpconstants.h>

#include <sputter/render/meshsubsystem.h>
#include <sputter/render/shaderstorage.h>
#include <sputter/render/uniform.h>

#include <sputter/input/inputsource.h>
#include <sputter/input/inputsubsystem.h>

#include <fpm/math.hpp>

using namespace sputter::render;
using namespace sputter::game;
using namespace sputter::assets;
using namespace sputter::math;
using namespace sputter::input;

const std::string Paddle::kCubeVertexShaderAssetName = "cube_vert";
const std::string Paddle::kCubeFragmentShaderAssetName = "cube_frag";
const std::string Paddle::kCubeShaderName = "cube_shader";

Paddle::Paddle(
    AssetStorageProvider* pStorageProvider,
    SubsystemProvider* pSubsystemProvider
) : Object(pStorageProvider, pSubsystemProvider)
{
    {
        sputter::render::Mesh::InitializationParameters params = {};
        CreateAndSetComponentByType<MeshSubsystem>(&m_pMeshComponent, params);
        if (!m_pMeshComponent)
        {
            sputter::system::LogAndFail("Failed to create mesh component in Paddle object.");
        }
    }

    {
        InputSource::InitializationParameters params;
        params.PlayerId = 0;
        CreateAndSetComponentByType<InputSubsystem>(&m_pInputSource, params);
        if (!m_pInputSource)
        {
            sputter::system::LogAndFail("Failed to create input source in Paddle object.");
        }
    }

    auto pShaderStorage = pStorageProvider->GetStorageByType<ShaderStorage>();
    if (!pShaderStorage->AddShaderFromShaderAssetNames(
        pStorageProvider->GetGeneralStorage(),
        kCubeVertexShaderAssetName,
        kCubeFragmentShaderAssetName,
        kCubeShaderName))
    {
        sputter::system::LogAndFail("Failed to add shader for character cube.");
    }

    m_spShader = pShaderStorage->FindShaderByName(kCubeShaderName);
    if (!m_spShader)
    {
        sputter::system::LogAndFail("Failed to retrieve shader for character cube.");
    }
}

void Paddle::Tick(FixedPoint deltaTime)
{
    FPVector3D velocity = FPVector3D::ZERO;
    const FixedPoint Speed = FixedPoint(400);
    if (m_pInputSource->IsInputHeld(static_cast<uint32_t>(PaddleArenaInput::INPUT_MOVE_UP)))
    {
        velocity += FPVector3D(0, 1, 0);
    }
    else if (m_pInputSource->IsInputHeld(static_cast<uint32_t>(PaddleArenaInput::INPUT_MOVE_DOWN)))
    {
        velocity += FPVector3D(0, -1, 0);
    }

    if (m_pInputSource->IsInputHeld(static_cast<uint32_t>(PaddleArenaInput::INPUT_MOVE_LEFT)))
    {
        velocity += FPVector3D(-1, 0, 0);
    }
    else if (m_pInputSource->IsInputHeld(static_cast<uint32_t>(PaddleArenaInput::INPUT_MOVE_RIGHT)))
    {
        velocity += FPVector3D(1, 0, 0);
    }

    if (velocity.Length() > FPZero)
    {
        const FPVector3D translation = m_localTransform.GetTranslation();
        m_localTransform.SetTranslation(translation + velocity.Normalized() * Speed * deltaTime);
        m_pMeshComponent->SetModelMatrix(m_localTransform.ToMat4());
    }

    // Do we need to do this on every tick?
    const uint32_t colorUniformHandle = m_spShader->GetUniform("color");
    Uniform<glm::vec3>::Set(colorUniformHandle, glm::vec3(1.0, 1.0, 1.0));
}

void Paddle::Initialize(
    sputter::math::FixedPoint cubeSize,
    sputter::math::FPVector3D location
    )
{
    m_localTransform.SetTranslation(location);

    using namespace sputter::math;
    // Cube's origin is at the geometric center.
    const FixedPoint HalfCubeSize = cubeSize / FPTwo;
    static const FPVector3D VertexPositions[] = {
        // Bottom face
        FPVector3D(-HalfCubeSize, -HalfCubeSize, -HalfCubeSize),
        FPVector3D( HalfCubeSize, -HalfCubeSize, -HalfCubeSize),
        FPVector3D( HalfCubeSize, -HalfCubeSize,  HalfCubeSize),
        FPVector3D(-HalfCubeSize, -HalfCubeSize,  HalfCubeSize),

        // Top face
        FPVector3D(-HalfCubeSize,  HalfCubeSize, -HalfCubeSize),
        FPVector3D( HalfCubeSize,  HalfCubeSize, -HalfCubeSize),
        FPVector3D( HalfCubeSize,  HalfCubeSize,  HalfCubeSize),
        FPVector3D(-HalfCubeSize,  HalfCubeSize,  HalfCubeSize),
    };

    static const FPVector3D VertexNormals[] = {
        // Bottom face
        FPVector3D(-HalfCubeSize, -HalfCubeSize, -HalfCubeSize).Normalized(),
        FPVector3D( HalfCubeSize, -HalfCubeSize, -HalfCubeSize).Normalized(),
        FPVector3D( HalfCubeSize, -HalfCubeSize,  HalfCubeSize).Normalized(),
        FPVector3D(-HalfCubeSize, -HalfCubeSize,  HalfCubeSize).Normalized(), 

        // Top face
        FPVector3D(-HalfCubeSize, HalfCubeSize, -HalfCubeSize).Normalized(),
        FPVector3D( HalfCubeSize, HalfCubeSize, -HalfCubeSize).Normalized(),
        FPVector3D( HalfCubeSize, HalfCubeSize,  HalfCubeSize).Normalized(),
        FPVector3D(-HalfCubeSize, HalfCubeSize,  HalfCubeSize).Normalized(), 
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
    const uint32_t NumIndices = sizeof(VertexIndices) / sizeof(VertexIndices[0]); 
    m_pMeshComponent->SetPositions(VertexPositions, NumVertices);
    m_pMeshComponent->SetNormals(VertexNormals, NumVertices);
    m_pMeshComponent->SetTextureCoordinates(VertexUVs, NumVertices);
    m_pMeshComponent->SetIndices(VertexIndices, NumIndices);
    m_pMeshComponent->SetShader(m_spShader);
    m_pMeshComponent->SetModelMatrix(m_localTransform.ToMat4());
}
