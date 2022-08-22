#include "paddle.h"
#include "paddlearena.h"
#include "objecttypes.h"

#include <sputter/assets/assetstorageprovider.h>

#include <sputter/math/fpconstants.h>

#include <sputter/render/meshsubsystem.h>
#include <sputter/render/shaderstorage.h>
#include <sputter/render/uniform.h>

#include <sputter/input/inputsource.h>
#include <sputter/input/inputsubsystem.h>

#include <sputter/physics/aabb.h>
#include <sputter/physics/collision.h>
#include <sputter/physics/collisionsubsystem.h>

#include <fpm/math.hpp>

using namespace sputter::render;
using namespace sputter::game;
using namespace sputter::assets;
using namespace sputter::math;
using namespace sputter::input;
using namespace sputter::physics;

const std::string Paddle::kPaddleVertexShaderAssetName = "cube_vert";
const std::string Paddle::kPaddleFragmentShaderAssetName = "cube_frag";
const std::string Paddle::kPaddleShaderName = "cube_shader";

Paddle::Paddle(
    uint32_t playerId,
    AssetStorageProvider* pStorageProvider,
    SubsystemProvider* pSubsystemProvider
) : Object(kPaddleArenaObjectTypePaddle, pStorageProvider, pSubsystemProvider)
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
        sputter::physics::Collision::InitializationParameters params = {};
        CreateAndSetComponentByType<CollisionSubsystem>(&m_pCollisionComponent, params);
        if (!m_pCollisionComponent)
        {
            sputter::system::LogAndFail("Failed to create collision component in Paddle object.");
        }

        m_pCollisionComponent->CollisionFlags = 1 << playerId;
    }

    {
        InputSource::InitializationParameters params;
        params.PlayerId = playerId;
        CreateAndSetComponentByType<InputSubsystem>(&m_pInputSource, params);
        if (!m_pInputSource)
        {
            sputter::system::LogAndFail("Failed to create input source in Paddle object.");
        }
    }

    auto pShaderStorage = pStorageProvider->GetStorageByType<ShaderStorage>();
    m_spShader = pShaderStorage->FindShaderByName(kPaddleShaderName);
    if (!m_spShader)
    {
        if (!pShaderStorage->AddShaderFromShaderAssetNames(
            pStorageProvider->GetGeneralStorage(),
            kPaddleVertexShaderAssetName,
            kPaddleFragmentShaderAssetName,
            kPaddleShaderName))
        {
            sputter::system::LogAndFail("Failed to add shader for paddle.");
        }
    }

    m_spShader = pShaderStorage->FindShaderByName(kPaddleShaderName);
    if (!m_spShader)
    {
        sputter::system::LogAndFail("Failed to retrieve shader for paddle.");
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
        TranslatePaddle(velocity.Normalized() * Speed * deltaTime);
    }
}

void Paddle::PostTick(FixedPoint deltaTime)
{
    for (CollisionResult& collisionResult : m_pCollisionComponent->CollisionsThisFrame)
    {
        const Collision& OtherCollision = collisionResult.pCollisionA == m_pCollisionComponent ?
            *collisionResult.pCollisionB : *collisionResult.pCollisionA;
        if (OtherCollision.pObject->GetType() == kPaddleArenaObjectTypeStage)
        {
            const ICollisionShape* pOtherShape = collisionResult.pCollisionA == m_pCollisionComponent ?
                collisionResult.pCollisionShapeB : collisionResult.pCollisionShapeA;

            // lol this is hideous
            AABB* pMyAABB = static_cast<AABB*>(m_pCollisionComponent->CollisionShapes.back());
            const AABB* pOtherAABB = static_cast<const AABB*>(pOtherShape);
            const FPVector3D Separation = pMyAABB->GetSeparation2D(pOtherAABB);

            TranslatePaddle(-Separation * (FPOne + FPEpsilon));
        }
    }
}

void Paddle::Initialize(
    sputter::math::FPVector2D dimensions,
    sputter::math::FPVector3D location
    )
{
    using namespace sputter::math;
    // Cube's origin is at the geometric center.
    const FixedPoint HalfCubeSize = FPOne / FPTwo;
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

    m_localTransform.SetScale(FPVector3D(dimensions.GetX(), dimensions.GetY(), FPOne));
    m_localTransform.SetTranslation(location);

    const uint32_t NumVertices = sizeof(VertexPositions) / sizeof(VertexPositions[0]); 
    const uint32_t NumIndices = sizeof(VertexIndices) / sizeof(VertexIndices[0]); 
    m_pMeshComponent->SetPositions(VertexPositions, NumVertices);
    m_pMeshComponent->SetNormals(VertexNormals, NumVertices);
    m_pMeshComponent->SetTextureCoordinates(VertexUVs, NumVertices);
    m_pMeshComponent->SetIndices(VertexIndices, NumIndices);
    m_pMeshComponent->SetShader(m_spShader);
    m_pMeshComponent->SetModelMatrix(m_localTransform.ToMat4());

    static const glm::vec3 White(1.0, 1.0, 1.0);
    m_pMeshComponent->SetMeshUniforms({ MeshUniformValue("color", UniformType::Vec3, &White) });

    // Now, set up collision geometry! Defined in *global* space at the moment. TODO: Fix that
    // Because of this, gotta update geometry on tick... D: D:
    //m_pCollisionComponent->CollisionFlags = 0x1; <-- Set during init according to player ID
    m_pCollisionComponent->pObject = this;
    m_pCollisionComponent->CollisionShapes.clear();
    
    const FPVector3D PaddleLowerLeft = FPVector3D(-dimensions.GetX() / FPTwo, -dimensions.GetY() / FPTwo, FPOne / FPTwo);
    AABB* pShape = new AABB(
         PaddleLowerLeft + location,
         FPVector3D(dimensions.GetX(), dimensions.GetY(), FPOne)
         );
    m_pCollisionComponent->CollisionShapes.push_back(pShape);
}

void Paddle::TranslatePaddle(const FPVector3D& translation)
{
    const FPVector3D CurrentTranslation = m_localTransform.GetTranslation();
    m_localTransform.SetTranslation(CurrentTranslation + translation);
    m_pMeshComponent->SetModelMatrix(m_localTransform.ToMat4());

    // Update collision transform as well
    const FPVector3D Scale = m_localTransform.GetScale();
    const FPVector3D PaddleLowerLeft = FPVector3D(-Scale.GetX() / FPTwo, -Scale.GetY() / FPTwo, FPOne / FPTwo);

    AABB* pMyAABB = static_cast<AABB*>(m_pCollisionComponent->CollisionShapes.back());
    pMyAABB->SetLowerLeft(PaddleLowerLeft + m_localTransform.GetTranslation());
}