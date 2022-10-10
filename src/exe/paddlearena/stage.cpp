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
using namespace sputter::core;

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
        if (CreateAndSetComponentByType<MeshSubsystem>(params) == kInvalidComponentHandle)
        {
            sputter::system::LogAndFail("Failed to create mesh component in stage object.");
        }
    }

    {
        sputter::physics::Collision::InitializationParameters params = {};
        if (CreateAndSetComponentByType<CollisionSubsystem>(params) == kInvalidComponentHandle)
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

    ShaderPtr spShader = pShaderStorage->FindShaderByName(kArenaShaderName);
    if (!spShader)
    {
        sputter::system::LogAndFail("Failed to retrieve shader for character cube.");
    }
}

void Stage::Tick(FixedPoint deltaTime)
{
    Mesh* pMeshComponent = GetComponentByType<Mesh>();
    RELEASE_CHECK(pMeshComponent, "Could not find mesh component for stage");
    pMeshComponent->SetModelMatrix(m_localTransform.ToMat4());
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
    if (!sputter::render::geometry::MakeFixedUnitCube(VertexPositions, 8, VertexNormals, 8, VertexUVs, 8, VertexIndices, 36))
    {
        sputter::system::LogAndFail("Failed to create a unit cube. What's up with that.");
    }

    auto pShaderStorage = m_pAssetStorageProvider->GetStorageByType<ShaderStorage>();
    ShaderPtr spShader = pShaderStorage->FindShaderByName(kArenaShaderName);

    Mesh* pMeshComponent = GetComponentByType<Mesh>();
    RELEASE_CHECK(pMeshComponent, "Could not find mesh component for stage");

    const uint32_t NumVertices = sizeof(VertexPositions) / sizeof(VertexPositions[0]); 
    const uint32_t NumIndices = sizeof(VertexIndices) / sizeof(VertexIndices[0]); 
    pMeshComponent->SetPositions(VertexPositions, NumVertices);
    pMeshComponent->SetNormals(VertexNormals, NumVertices);
    pMeshComponent->SetTextureCoordinates(VertexUVs, NumVertices);
    pMeshComponent->SetIndices(VertexIndices, NumIndices);
    pMeshComponent->SetShader(spShader);
    pMeshComponent->SetModelMatrix(m_localTransform.ToMat4());

    static const glm::vec3 Gray(0.5, 0.5, 0.5);
    pMeshComponent->SetMeshUniforms({ MeshUniformValue("color", UniformType::Vec3, &Gray) });

    // Now, set up collision geometry! Defined in *global* space at the moment. TODO: Fix that
    Collision* pCollision = GetComponentByType<Collision>();
    RELEASE_CHECK(pCollision, "Could not find collision component for stage");
    pCollision->CollisionFlags = 0b111;
    pCollision->pObject = this;
    pCollision->CollisionShapes.clear();

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
    pCollision->CollisionShapes.push_back(new AABB(LeftLowerLeft, SideCollisionExtents));
    pCollision->CollisionShapes.push_back(new AABB(RightLowerLeft, SideCollisionExtents));

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
    pCollision->CollisionShapes.push_back(new AABB(TopLowerLeft, TopAndBottomCollisionExtents));
    pCollision->CollisionShapes.push_back(new AABB(BottomLowerLeft, TopAndBottomCollisionExtents));
}