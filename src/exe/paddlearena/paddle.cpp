#include "paddle.h"
#include "paddlearena.h"
#include "objecttypes.h"
#include "gameconstants.h"
#include "gamestate.h"

#include <sputter/assets/assetstorageprovider.h>

#include <sputter/math/fpconstants.h>

#include <sputter/render/meshsubsystem.h>
#include <sputter/render/shaderstorage.h>
#include <sputter/render/uniform.h>
#include <sputter/render/geometry.h>

#include <sputter/input/inputsource.h>
#include <sputter/input/inputsubsystem.h>

#include <sputter/physics/aabb.h>
#include <sputter/physics/collision.h>
#include <sputter/physics/collisionsubsystem.h>

#include <sputter/core/check.h>
#include <sputter/system/system.h>

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
    GameState* pGameState,
    uint32_t playerId,
    AssetStorageProvider* pStorageProvider,
    SubsystemProvider* pSubsystemProvider
) : Object(kPaddleArenaObjectTypePaddle, pStorageProvider, pSubsystemProvider),
    m_pGameState(pGameState),
    m_playerId(playerId)
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
    if (m_playerId == 1)
    {
        // TEMP: simple "AI" for testing purposes, for the second paddle
        const FixedPoint PaddleY = m_localTransform.GetTranslation().GetY();
        const FixedPoint BallY = m_pGameState->TheBall.GetPosition().GetY();
        const FixedPoint VerticalDelta = BallY - PaddleY;

        const FixedPoint TravelThreshold(5);
        if (VerticalDelta > TravelThreshold)
        {
            velocity += FPVector3D(0, 1, 0);
        }
        else if (VerticalDelta < -TravelThreshold)
        {
            velocity += FPVector3D(0, -1, 0);
        }
    }
    else
    {
        if (m_pInputSource->IsInputHeld(static_cast<uint32_t>(PaddleArenaInput::INPUT_MOVE_UP)))
        {
            velocity += FPVector3D(0, 1, 0);
        }
        else if (m_pInputSource->IsInputHeld(static_cast<uint32_t>(PaddleArenaInput::INPUT_MOVE_DOWN)))
        {
            velocity += FPVector3D(0, -1, 0);
        }

        if (IsBallAttached())
        {
            if (m_pInputSource->IsInputPressed(static_cast<uint32_t>(PaddleArenaInput::INPUT_SERVE)))
            {
                DetachBall(GetFacingDirection());
            }
        }
    }

// Just vertical movement for now
#if 0
    if (m_pInputSource->IsInputHeld(static_cast<uint32_t>(PaddleArenaInput::INPUT_MOVE_LEFT)))
    {
        velocity += FPVector3D(-1, 0, 0);
    }
    else if (m_pInputSource->IsInputHeld(static_cast<uint32_t>(PaddleArenaInput::INPUT_MOVE_RIGHT)))
    {
        velocity += FPVector3D(1, 0, 0);
    }
#endif

    if (velocity.Length() > FPZero)
    {
        TranslatePaddle(velocity.Normalized() * gameconstants::PaddleSpeed * deltaTime);
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

    static FPVector3D VertexPositions[8];
    static FPVector3D VertexNormals[8];
    static FPVector2D VertexUVs[8];
    static int VertexIndices[36];
    if (!sputter::render::geometry::MakeFixedUnitCube(VertexPositions, 8, VertexNormals, 8, VertexUVs, 8, VertexIndices, 36))
    {
        sputter::system::LogAndFail("Failed to create a unit cube. What's up with that.");
    }

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

    m_pCollisionComponent->CollisionFlags = (1 << m_playerId);
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

    if (IsBallAttached())
    {
        m_pAttachedBall->TranslateBall(translation);
    }
}

void Paddle::AttachBall(Ball* pBall)
{
    RELEASE_CHECK(!m_pAttachedBall, "Attempting to attach ball to paddle, but a ball is already attached.");
    m_pAttachedBall = pBall;

    // Temporarily avoid colliding with the ball
    m_pAttachedBall->SetCanCollideWithPaddle(m_playerId, false);

    // Figure out where to place the ball, now that it's attached.
    const FixedPoint MyHalfWidth = GetDimensions().GetX() / FPTwo;
    const FixedPoint BallHalfWidth = m_pAttachedBall->GetDimensions().GetX() / FPTwo;

    const FPVector2D NewBallLocation2D = GetPosition() + (GetFacingDirection() * (MyHalfWidth + BallHalfWidth));
    const FPVector3D NewBallLocation3D(NewBallLocation2D.GetX(), NewBallLocation2D.GetY(), -FPOneHalf);
    const FPVector2D& BallStartVelocity = FPVector2D::ZERO;
    m_pAttachedBall->Reset(NewBallLocation3D, BallStartVelocity);
}

void Paddle::DetachBall(const FPVector2D& detachVelocity)
{
    RELEASE_CHECK(m_pAttachedBall, "Attempting to detach ball from paddle, but no ball is attached.");

    // Reenable collision
    m_pAttachedBall->SetCanCollideWithPaddle(m_playerId, true);
    m_pAttachedBall->SetVelocity(detachVelocity);
    m_pAttachedBall = nullptr;
}

bool Paddle::IsBallAttached() const
{
    return m_pAttachedBall != nullptr;
}

FPVector2D Paddle::GetFacingDirection() const
{
    return (m_playerId == 0) ? FPVector2D::RIGHT : FPVector2D::LEFT;
}

FPVector2D Paddle::GetPosition() const
{
    const FPVector3D& Translation = m_localTransform.GetTranslation();
    return FPVector2D(Translation.GetX(), Translation.GetY());
}

FPVector2D Paddle::GetDimensions() const
{
    const FPVector3D& Scale = m_localTransform.GetScale();
    return FPVector2D(Scale.GetX(), Scale.GetY());
}
