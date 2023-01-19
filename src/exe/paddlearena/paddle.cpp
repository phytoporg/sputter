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

using namespace sputter::core;
using namespace sputter::render;
using namespace sputter::game;
using namespace sputter::assets;
using namespace sputter::math;
using namespace sputter::input;
using namespace sputter::physics;

const std::string Paddle::kPaddleVertexShaderAssetName = "cube_vert";
const std::string Paddle::kPaddleFragmentShaderAssetName = "cube_frag";
const std::string Paddle::kPaddleShaderName = "cube_shader";

Paddle::Paddle(uint32_t playerId) 
    : Object(kPaddleArenaObjectTypePaddle), m_playerId(playerId)
{
    {
        sputter::render::Mesh::InitializationParameters params = {};

        sputter::core::ComponentHandle meshComponentHandle;
        if (CreateAndSetComponentByType<MeshSubsystem>(params) == kInvalidComponentHandle)
        {
            sputter::system::LogAndFail("Failed to create mesh component in Paddle object.");
        }
    }

    {
        sputter::physics::Collision::InitializationParameters params = {};
        if (CreateAndSetComponentByType<CollisionSubsystem>(params) == kInvalidComponentHandle)
        {
            sputter::system::LogAndFail("Failed to create collision component in Paddle object.");
        }
    }

    {
        InputSource::InitializationParameters params;
        params.PlayerId = playerId;
        if (CreateAndSetComponentByType<InputSubsystem>(params) == kInvalidComponentHandle)
        {
            sputter::system::LogAndFail("Failed to create input source in Paddle object.");
        }
    }

    auto pAssetStorageProvider = AssetStorageProvider::GetAssetStorageProviderAddress();
    auto pShaderStorage = pAssetStorageProvider->GetStorageByType<ShaderStorage>();
    m_shaderHandle = pShaderStorage->FindShaderHandleByName(kPaddleShaderName);
    if (m_shaderHandle == kInvalidResourceHandle)
    {
        if (!pShaderStorage->AddShaderFromShaderAssetNames(
            pAssetStorageProvider->GetGeneralStorage(),
            kPaddleVertexShaderAssetName,
            kPaddleFragmentShaderAssetName,
            kPaddleShaderName))
        {
            sputter::system::LogAndFail("Failed to add shader for paddle.");
        }
    }

    m_shaderHandle = pShaderStorage->FindShaderHandleByName(kPaddleShaderName);
    if (m_shaderHandle == kInvalidResourceHandle)
    {
        sputter::system::LogAndFail("Failed to retrieve shader for paddle.");
    }
}

void Paddle::Tick(FixedPoint deltaTime)
{
    FPVector3D velocity = FPVector3D::ZERO;
    if (m_playerId == 1)
    {
        if (IsBallAttached())
        {
            DetachBall(GetFacingDirection());
        }

        // TEMP: simple "AI" for testing purposes, for the second paddle
        GameState* pGameState = GameState::GetGameStateAddress();
        const FixedPoint PaddleY = m_localTransform.GetTranslation().GetY();
        const FixedPoint PaddleX = m_localTransform.GetTranslation().GetX();
        const FixedPoint BallY = pGameState->TheBall.GetPosition().GetY();
        const FixedPoint BallX = pGameState->TheBall.GetPosition().GetX();
        const FixedPoint VerticalDelta = BallY - PaddleY;
        const FixedPoint HorizontalDelta = fpm::abs(BallX - PaddleX);

        const FixedPoint WaitDistance(gameconstants::ArenaDimensions.GetX() / FPTwo);
        // If the ball is too far away, just chill
        if (HorizontalDelta < WaitDistance)
        {
            const FixedPoint TravelThreshold(15);
            if (VerticalDelta > TravelThreshold)
            {
                velocity += FPVector3D(0, 1, 0);
            }
            else if (VerticalDelta < -TravelThreshold)
            {
                velocity += FPVector3D(0, -1, 0);
            }
        }
    }
    else
    {
        InputSource* pInputSource = GetComponentByType<InputSource>();
        if (pInputSource->IsInputHeld(static_cast<uint32_t>(PaddleArenaInput::INPUT_MOVE_UP)))
        {
            velocity += FPVector3D(0, 1, 0);
        }
        else if (pInputSource->IsInputHeld(static_cast<uint32_t>(PaddleArenaInput::INPUT_MOVE_DOWN)))
        {
            velocity += FPVector3D(0, -1, 0);
        }

        if (pInputSource->IsInputPressed(static_cast<uint32_t>(PaddleArenaInput::INPUT_DASH)))
        {
            if (!velocity.IsZero() && m_dashVelocityY == FPZero)
            {
                m_dashVelocityY = velocity.GetY() * gameconstants::PaddleDashSpeed;
            }
        }

        if (IsBallAttached())
        {
            if (pInputSource->IsInputPressed(static_cast<uint32_t>(PaddleArenaInput::INPUT_SERVE)))
            {
                const FPVector2D Velocity2D(velocity.GetX(), velocity.GetY());
                DetachBall(GetFacingDirection() + Velocity2D);
            }
        }
    }

// Just vertical movement for now
#if 0
    if (pInputSource->IsInputHeld(static_cast<uint32_t>(PaddleArenaInput::INPUT_MOVE_LEFT)))
    {
        velocity += FPVector3D(-1, 0, 0);
    }
    else if (pInputSource->IsInputHeld(static_cast<uint32_t>(PaddleArenaInput::INPUT_MOVE_RIGHT)))
    {
        velocity += FPVector3D(1, 0, 0);
    }
#endif

    if (m_dashVelocityY != FPZero)
    {
        TranslatePaddle(FPVector3D::UP * m_dashVelocityY * deltaTime);
    }
    else if (velocity.Length() > FPZero)
    {
        TranslatePaddle(velocity * gameconstants::PaddleSpeed * deltaTime);
    }

    // Dash velocity "friction"
    if (m_dashVelocityY != FPZero)
    {
        // TODO: A forreal function for tuning duration, change, recovery, etc
        m_dashVelocityY -= (m_dashVelocityY * FixedPoint(6) * deltaTime);
        if (fpm::abs(m_dashVelocityY) < gameconstants::PaddleSpeed)
        {
            m_dashVelocityY = FPZero;
        }
    }
}

void Paddle::PostTick(FixedPoint deltaTime)
{
    Collision* pCollisionComponent = GetComponentByType<Collision>();
    RELEASE_CHECK(pCollisionComponent, "Could not find collision component on paddle")
    for (size_t i = 0; i < pCollisionComponent->NumCollisionsThisFrame; ++i)
    {
        const CollisionResult& collisionResult = pCollisionComponent->CollisionsThisFrame[i];
        const Collision& OtherCollision = collisionResult.pCollisionA == pCollisionComponent ?
            *collisionResult.pCollisionB : *collisionResult.pCollisionA;
        if (OtherCollision.pObject->GetType() == kPaddleArenaObjectTypeStage)
        {
            const ICollisionShape* pOtherShape = collisionResult.pCollisionA == pCollisionComponent ?
                collisionResult.pCollisionShapeB : collisionResult.pCollisionShapeA;

            // lol this is hideous
            AABB* pMyAABB = &pCollisionComponent->CollisionShapes[pCollisionComponent->NumCollisionShapes - 1];
            const AABB* pOtherAABB = static_cast<const AABB*>(pOtherShape);
            const FPVector3D Separation = pMyAABB->GetSeparation2D(pOtherAABB);

            TranslatePaddle(-Separation * (FPOne + FPEpsilon));
        }
    }
}

bool Paddle::Serialize(void* pBuffer, size_t* pBytesWrittenOut, size_t maxBytes)
{
    WRITE(m_localTransform, pBuffer, *pBytesWrittenOut, maxBytes);
    *pBytesWrittenOut += sizeof(m_localTransform);

    WRITE(m_initialLocation, pBuffer, *pBytesWrittenOut, maxBytes);
    *pBytesWrittenOut += sizeof(m_initialLocation);

    WRITE(m_dashVelocityY, pBuffer, *pBytesWrittenOut, maxBytes);
    *pBytesWrittenOut += sizeof(m_dashVelocityY);

    WRITE(m_ballAttached, pBuffer, *pBytesWrittenOut, maxBytes);
    *pBytesWrittenOut += sizeof(m_ballAttached);

    return true;
}

bool Paddle::Deserialize(void* pBuffer, size_t* pBytesReadOut, size_t maxBytes)
{    
    READ(m_localTransform, pBuffer, *pBytesReadOut, maxBytes);
    *pBytesReadOut += sizeof(m_localTransform);

    READ(m_initialLocation, pBuffer, *pBytesReadOut, maxBytes);
    *pBytesReadOut += sizeof(m_initialLocation);

    READ(m_dashVelocityY, pBuffer, *pBytesReadOut, maxBytes);
    *pBytesReadOut += sizeof(m_dashVelocityY);

    READ(m_ballAttached, pBuffer, *pBytesReadOut, maxBytes);
    *pBytesReadOut += sizeof(m_ballAttached);

    return true;
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

    auto pAssetStorageProvider = AssetStorageProvider::GetAssetStorageProviderAddress();
    auto pShaderStorage = pAssetStorageProvider->GetStorageByType<ShaderStorage>();
    ShaderPtr spShader = pShaderStorage->GetShaderFromHandle(m_shaderHandle);

    Mesh* pMesh = GetComponentByType<Mesh>();
    RELEASE_CHECK(pMesh, "Could not find mesh component")
    pMesh->SetPositions(VertexPositions, NumVertices);
    pMesh->SetNormals(VertexNormals, NumVertices);
    pMesh->SetTextureCoordinates(VertexUVs, NumVertices);
    pMesh->SetIndices(VertexIndices, NumIndices);
    pMesh->SetShader(spShader);
    pMesh->SetModelMatrix(m_localTransform.ToMat4());

    static const glm::vec3 White(1.0, 1.0, 1.0);
    pMesh->SetMeshUniforms({ MeshUniformValue("color", UniformType::Vec3, &White) });

    // Now, set up collision geometry! Defined in *global* space at the moment. TODO: Fix that
    // Because of this, gotta update geometry on tick... D: D:

    Collision* pCollisionComponent = GetComponentByType<Collision>();
    RELEASE_CHECK(pCollisionComponent, "Could not find collision component on paddle")
    pCollisionComponent->CollisionFlags = 0b111;
    pCollisionComponent->pObject = this;
    pCollisionComponent->NumCollisionShapes = 0;
    
    const FPVector3D PaddleLowerLeft = FPVector3D(-dimensions.GetX() / FPTwo, -dimensions.GetY() / FPTwo, FPOne / FPTwo);
    pCollisionComponent->CollisionShapes[pCollisionComponent->NumCollisionShapes++] = AABB(
         PaddleLowerLeft + location,
         FPVector3D(dimensions.GetX(), dimensions.GetY(), FPOne)
         );

}

void Paddle::TranslatePaddle(const FPVector3D& translation)
{
    const FPVector3D CurrentTranslation = m_localTransform.GetTranslation();
    m_localTransform.SetTranslation(CurrentTranslation + translation);

    Mesh* pMesh = GetComponentByType<Mesh>();
    RELEASE_CHECK(pMesh, "Could not find mesh component")
    pMesh->SetModelMatrix(m_localTransform.ToMat4());

    // Update collision transform as well
    const FPVector3D Scale = m_localTransform.GetScale();
    const FPVector3D PaddleLowerLeft = FPVector3D(-Scale.GetX() / FPTwo, -Scale.GetY() / FPTwo, FPOne / FPTwo);

    Collision* pCollisionComponent = GetComponentByType<Collision>();
    RELEASE_CHECK(pCollisionComponent, "Could not find collision component on paddle")
    AABB& myAABB = pCollisionComponent->CollisionShapes[pCollisionComponent->NumCollisionShapes - 1];
    myAABB.SetLowerLeft(PaddleLowerLeft + m_localTransform.GetTranslation());

    if (IsBallAttached())
    {
        GameState* pGameState = GameState::GetGameStateAddress();
        pGameState->TheBall.TranslateBall(translation);
    }
}

void Paddle::AttachBall(Ball* pBall)
{
    // RELEASE_CHECK() may be overkill here, but this is currently unexpected
    RELEASE_CHECK(!IsBallAttached(), "Attempting to attach ball to paddle, but a ball is already attached.");

    m_ballAttached = true;

    // Temporarily avoid colliding with the ball
    pBall->SetCanCollideWithPaddle(m_playerId, false);

    // Figure out where to place the ball, now that it's attached.
    const FixedPoint MyHalfWidth = GetDimensions().GetX() / FPTwo;
    const FixedPoint BallHalfWidth = pBall->GetDimensions().GetX() / FPTwo;

    const FPVector2D NewBallLocation2D = GetPosition() + (GetFacingDirection() * (MyHalfWidth + BallHalfWidth));
    const FPVector3D NewBallLocation3D(NewBallLocation2D.GetX(), NewBallLocation2D.GetY(), -FPOneHalf);
    const FPVector2D& BallStartVelocity = FPVector2D::ZERO;
    pBall->Reset(NewBallLocation3D, BallStartVelocity);
}

void Paddle::DetachBall(const FPVector2D& detachVelocity)
{
    RELEASE_CHECK(IsBallAttached(), "Attempting to detach ball from paddle, but no ball is attached.");

    // Reenable collision
    GameState* pGameState = GameState::GetGameStateAddress();
    RELEASE_CHECK(pGameState, "Failed to get game state address");
    Ball* pBall = &pGameState->TheBall;

    pBall->SetCanCollideWithPaddle(m_playerId, true);
    pBall->SetVelocity(detachVelocity);
    m_ballAttached = false;
}

bool Paddle::IsBallAttached() const
{
    return m_ballAttached;
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

uint32_t Paddle::GetPlayerID() const
{
    return m_playerId;
}
