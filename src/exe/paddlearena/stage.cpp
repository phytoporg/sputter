#include "stage.h"
#include "objecttypes.h"

#include <string>

#include <sputter/math/fpconstants.h>

#include <sputter/assets/assetstorageprovider.h>

#include <sputter/game/subsystemprovider.h>

#include <sputter/render/shaderstorage.h>
#include <sputter/render/meshsubsystem.h>
#include <sputter/render/mesh.h>
#include <sputter/render/geometry.h>

#include <sputter/physics/aabb.h>
#include <sputter/physics/collision.h>
#include <sputter/physics/collisionsubsystem.h>

using namespace sputter::game;
using namespace sputter::math;
using namespace sputter::assets;
using namespace sputter::render;
using namespace sputter::physics;

const std::string Stage::kArenaVertexShaderAssetName = "cube_vert";
const std::string Stage::kArenaFragmentShaderAssetName = "cube_frag";
const std::string Stage::kArenaShaderName = "arena_shader";

Stage::Stage(
    AssetStorageProvider* pStorageProvider,
    SubsystemProvider* pSubsystemProvider
) : Object(kPaddleArenaObjectTypeStage, pStorageProvider, pSubsystemProvider)
{
    {
        sputter::render::Mesh::InitializationParameters params = {};
        CreateAndSetComponentByType<MeshSubsystem>(&m_pMeshComponent, params);
        if (!m_pMeshComponent)
        {
            sputter::system::LogAndFail("Failed to create mesh component in stage object.");
        }
    }

    {
        sputter::physics::Collision::InitializationParameters params = {};
        CreateAndSetComponentByType<CollisionSubsystem>(&m_pCollisionComponent, params);
        if (!m_pCollisionComponent)
        {
            sputter::system::LogAndFail("Failed to create collision component in stage object.");
        }
    }

    auto pShaderStorage = pStorageProvider->GetStorageByType<ShaderStorage>();
    if (!pShaderStorage->AddShaderFromShaderAssetNames(
        pStorageProvider->GetGeneralStorage(),
        kArenaVertexShaderAssetName,
        kArenaFragmentShaderAssetName,
        kArenaShaderName))
    {
        sputter::system::LogAndFail("Failed to add shader for the arena.");
    }

    m_spShader = pShaderStorage->FindShaderByName(kArenaShaderName);
    if (!m_spShader)
    {
        sputter::system::LogAndFail("Failed to retrieve shader for character cube.");
    }
}

void Stage::Tick(FixedPoint deltaTime)
{
    m_pMeshComponent->SetModelMatrix(m_localTransform.ToMat4());
}

void Stage::Initialize(FPVector2D stageDimensions)
{
    m_localTransform.SetScale(
        FPVector3D(
            stageDimensions.GetX(),
            stageDimensions.GetY(),
            FixedPoint(1)));
    m_localTransform.SetTranslation(FPVector3D(FPZero, FPZero, FixedPoint(0.5)));

    static FPVector3D VertexPositions[8];
    static FPVector3D VertexNormals[8];
    static FPVector2D VertexUVs[8];
    static int VertexIndices[36];
    if (!sputter::render::geometry::MakeUnitCube(VertexPositions, 8, VertexNormals, 8, VertexUVs, 8, VertexIndices, 36))
    {
        sputter::system::LogAndFail("Failed to create a unit cube. What's up with that.");
    }

    const uint32_t NumVertices = sizeof(VertexPositions) / sizeof(VertexPositions[0]); 
    const uint32_t NumIndices = sizeof(VertexIndices) / sizeof(VertexIndices[0]); 
    m_pMeshComponent->SetPositions(VertexPositions, NumVertices);
    m_pMeshComponent->SetNormals(VertexNormals, NumVertices);
    m_pMeshComponent->SetTextureCoordinates(VertexUVs, NumVertices);
    m_pMeshComponent->SetIndices(VertexIndices, NumIndices);
    m_pMeshComponent->SetShader(m_spShader);
    m_pMeshComponent->SetModelMatrix(m_localTransform.ToMat4());

    static const glm::vec3 Gray(0.5, 0.5, 0.5);
    m_pMeshComponent->SetMeshUniforms({ MeshUniformValue("color", UniformType::Vec3, &Gray) });

    // Now, set up collision geometry! Defined in *global* space at the moment. TODO: Fix that
    m_pCollisionComponent->CollisionFlags = 0x11;
    m_pCollisionComponent->pObject = this;
    m_pCollisionComponent->CollisionShapes.clear();

    // Left and right AABBs
    const FPVector3D SideCollisionExtents(
        FixedPoint(100),
        stageDimensions.GetY(),
        FixedPoint(50)
    );
    const FPVector3D LeftLowerLeft(
        -(stageDimensions.GetX() / FPTwo) - SideCollisionExtents.GetX(), 
        -(stageDimensions.GetY() / FPTwo),
        -FPTwo);
    const FPVector3D RightLowerLeft(
        (stageDimensions.GetX() / FPTwo), 
        -(stageDimensions.GetY() / FPTwo),
        -FPTwo);
    m_pCollisionComponent->CollisionShapes.push_back(new AABB(LeftLowerLeft, SideCollisionExtents));
    m_pCollisionComponent->CollisionShapes.push_back(new AABB(RightLowerLeft, SideCollisionExtents));

    // Top and bottom AABBs
    const FPVector3D TopAndBottomCollisionExtents(
        stageDimensions.GetX(),
        FixedPoint(100),
        FixedPoint(50)
    );
    const FPVector3D TopLowerLeft(
        -(stageDimensions.GetX() / FPTwo), 
        (stageDimensions.GetY() / FPTwo),
        -FPTwo);
    const FPVector3D BottomLowerLeft(
        -(stageDimensions.GetX() / FPTwo), 
        -(stageDimensions.GetY() / FPTwo) - TopAndBottomCollisionExtents.GetY(),
        -FPTwo);
    m_pCollisionComponent->CollisionShapes.push_back(new AABB(TopLowerLeft, TopAndBottomCollisionExtents));
    m_pCollisionComponent->CollisionShapes.push_back(new AABB(BottomLowerLeft, TopAndBottomCollisionExtents));
}