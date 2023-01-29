#include "ball.h"
#include "paddle.h"
#include "paddlearena.h"
#include "objecttypes.h"
#include "gameconstants.h"

#include <sputter/system/system.h>

#include <sputter/math/fpconstants.h>
#include <sputter/math/fpvector2d.h>
#include <sputter/math/fpvector3d.h>

#include <sputter/render/meshsubsystem.h>
#include <sputter/render/shaderstorage.h>
#include <sputter/render/uniform.h>
#include <sputter/render/geometry.h>
#include <sputter/render/drawshapes.h>
#include <sputter/render/mesh.h>

#include <sputter/physics/aabb.h>
#include <sputter/physics/collision.h>
#include <sputter/physics/collisionsubsystem.h>

#include <sputter/game/objectstorage.h>
#include <sputter/game/subsystemprovider.h>

#include <sputter/core/check.h>
#include <sputter/core/debugsettings.h>

#include <sputter/containers/arrayvector.h>

#include <sputter/system/system.h>

#include <fpm/math.hpp>

using namespace sputter::render;
using namespace sputter::game;
using namespace sputter::assets;
using namespace sputter::math;
using namespace sputter::physics;
using namespace sputter::core;
using namespace sputter::containers;

// Same ol' same ol' for now
const std::string Ball::kBallVertexShaderAssetName = "cube_vert";
const std::string Ball::kBallFragmentShaderAssetName = "cube_frag";
const std::string Ball::kBallShaderName = "cube_shader";

Ball::Ball() : Object(kPaddleArenaObjectTypeBall)
{
    {
        sputter::render::Mesh::InitializationParameters params = {};
        if (CreateAndSetComponentByType<MeshSubsystem>(params) == kInvalidComponentHandle)
        {
            sputter::system::LogAndFail("Failed to create mesh component in Ball object.");
        }
    }

    {
        sputter::physics::Collision::InitializationParameters params = {};
        if (CreateAndSetComponentByType<CollisionSubsystem>(params) == kInvalidComponentHandle)
        {
            sputter::system::LogAndFail("Failed to create collision component in Ball object.");
        }
    }

    auto pAssetStorageProvider = AssetStorageProvider::GetAssetStorageProviderAddress();
    auto pShaderStorage = pAssetStorageProvider->GetStorageByType<ShaderStorage>();
    ResourceHandle shaderHandle = pShaderStorage->FindShaderHandleByName(kBallShaderName);
    if (shaderHandle == kInvalidResourceHandle)
    {
        if (!pShaderStorage->AddShaderFromShaderAssetNames(
            pAssetStorageProvider->GetGeneralStorage(),
            kBallVertexShaderAssetName,
            kBallFragmentShaderAssetName,
            kBallShaderName))
        {
            sputter::system::LogAndFail("Failed to add shader for ball.");
        }
    }

    shaderHandle = pShaderStorage->FindShaderHandleByName(kBallShaderName);
    if (shaderHandle == kInvalidResourceHandle)
    {
        sputter::system::LogAndFail("Failed to retrieve shader for ball.");
    }
}

void Ball::Tick(sputter::math::FixedPoint deltaTime)
{
    if (IsDead())
    {
        // Nothing to do when dead.
        return;
    }

    const FixedPoint Speed = gameconstants::BallSpeed;
    if (!m_travelVector.IsZero())
    {
        const FPVector2D TravelNormalized(m_travelVector.Normalized());
        TranslateBall(FPVector3D(TravelNormalized.GetX(), TravelNormalized.GetY(), FPZero) * Speed * deltaTime);
    }
}

void Ball::PostTick(sputter::math::FixedPoint deltaTime)
{
    if (IsDead())
    {
        // Nothing to do when dead.
        return;
    }

    SubsystemProvider* pSubsystemProvider = SubsystemProvider::GetSubsystemProviderAddress();
    CollisionSubsystem* pCollisionSubsystem = pSubsystemProvider->GetSubsystemByType<CollisionSubsystem>();

    Collision* pCollision = GetComponentByType<Collision>();
    RELEASE_CHECK(pCollision, "Could not get collision component on Ball");

    ArrayVector<CollisionResult, CollisionSubsystem::kMaxCollisionResults> collisionsThisFrame;
    if (!pCollisionSubsystem->GetCollisionResultsForThisFrame(pCollision, &collisionsThisFrame))
    {
        sputter::system::LogAndFail("Failed to query for collisions this frame");
    }
    
    for (size_t i = 0; i < collisionsThisFrame.Size(); ++i)
    {
        const CollisionResult& collisionResult = collisionsThisFrame[i];
        RELEASE_CHECK(collisionResult.pCollisionA && collisionResult.pCollisionB, "Invalid collision result!");

        const Collision& OtherCollision = collisionResult.pCollisionA == pCollision ?
            *collisionResult.pCollisionB : *collisionResult.pCollisionA;

        Object* pOtherObject = objectstorage::FindObject(OtherCollision.ObjectHandle);
        RELEASE_CHECK(pOtherObject, "Could not find other collision object");
        if (pOtherObject->GetType() == kPaddleArenaObjectTypeStage)
        {
            const AABB* pOtherAABB = collisionResult.pCollisionA == pCollision ?
                collisionResult.pCollisionShapeB : collisionResult.pCollisionShapeA;

            // lol this is hideous
            AABB* pMyAABB = &pCollision->CollisionShapes[pCollision->NumCollisionShapes - 1];
            const FPVector3D Separation = pMyAABB->GetSeparation2D(*pOtherAABB);

            if (fpm::abs(Separation.GetY()) > FPZero)
            {
                // Colliding with top of arena, negate y-axis travel and correct separation
                m_travelVector.SetY(-m_travelVector.GetY());
                TranslateBall(-Separation * (FPOne + FPEpsilon));
            }
            else 
            {
                // Colliding with side of the arena. Time to DIE. Abruptly for the time being.
                m_isAlive = false;

                Mesh* pMeshComponent = GetComponentByType<Mesh>();
                RELEASE_CHECK(pMeshComponent, "Could not find mesh component on ball");
                pMeshComponent->SetVisibility(false);
            }
        }
        else if (pOtherObject->GetType() == kPaddleArenaObjectTypePaddle)
        {
            const AABB* pOtherAABB = collisionResult.pCollisionA == pCollision ?
                collisionResult.pCollisionShapeB : collisionResult.pCollisionShapeA;

            // lol this is also hideous
            AABB* pMyAABB = &pCollision->CollisionShapes[pCollision->NumCollisionShapes - 1];
            const FPVector3D Separation = pMyAABB->GetSeparation2D(*pOtherAABB);

            if (fpm::abs(Separation.GetX()) > FPZero)
            {
                // We only care about colliding with the *front* face when colliding with
                // the side of a paddle.
                Paddle* pPaddle = reinterpret_cast<Paddle*>(
                        objectstorage::FindObject(OtherCollision.ObjectHandle)
                    );
                RELEASE_CHECK(pPaddle, "Invalid paddle object pointer");
                const int8_t SeparationXSign = Separation.GetX() > FPZero ? 1 : -1;
                const int8_t PaddleDirectionXSign = pPaddle->GetFacingDirection().GetX() > FPZero ? 1 : -1;
                if (SeparationXSign != PaddleDirectionXSign)
                {
                    // Correct position based on separation
                    TranslateBall(-Separation * (FPOne + FPEpsilon));
                    m_travelVector = ComputeBallDeflectionFromPaddle(pPaddle);
                }
            }
            else if (fpm::abs(Separation.GetY()) > FPZero)
            {
                // Colliding with side of the paddle. Might as well bounce here too?
                m_travelVector.SetY(-m_travelVector.GetY());
                TranslateBall(-Separation * (FPOne + FPEpsilon));
            }
        }
    }
}

bool Ball::Serialize(void* pBuffer, size_t* pBytesWrittenOut, size_t maxBytes)
{
    WRITE(m_localTransform, pBuffer, *pBytesWrittenOut, maxBytes);
    *pBytesWrittenOut += sizeof(m_localTransform);

    WRITE(m_initialLocation, pBuffer, *pBytesWrittenOut, maxBytes);
    *pBytesWrittenOut += sizeof(m_initialLocation);

    WRITE(m_travelVector, pBuffer, *pBytesWrittenOut, maxBytes);
    *pBytesWrittenOut += sizeof(m_travelVector);

    WRITE(m_isAlive, pBuffer, *pBytesWrittenOut, maxBytes);
    *pBytesWrittenOut += sizeof(m_isAlive);

    WRITE(m_isInitialized, pBuffer, *pBytesWrittenOut, maxBytes);
    *pBytesWrittenOut += sizeof(m_isInitialized);

    return true;
}

bool Ball::Deserialize(void* pBuffer, size_t* pBytesReadOut, size_t maxBytes)
{
    READ(m_localTransform, pBuffer, *pBytesReadOut, maxBytes);
    *pBytesReadOut += sizeof(m_localTransform);

    READ(m_initialLocation, pBuffer, *pBytesReadOut, maxBytes);
    *pBytesReadOut += sizeof(m_initialLocation);

    READ(m_travelVector, pBuffer, *pBytesReadOut, maxBytes);
    *pBytesReadOut += sizeof(m_travelVector);

    READ(m_isAlive, pBuffer, *pBytesReadOut, maxBytes);
    *pBytesReadOut += sizeof(m_isAlive);

    READ(m_isInitialized, pBuffer, *pBytesReadOut, maxBytes);
    *pBytesReadOut += sizeof(m_isInitialized);

    // Force a re-render
    Mesh* pMesh = GetComponentByType<Mesh>();
    pMesh->MarkDirty();

    return true;
}

bool Ball::IsInitialized() const
{
    return m_isInitialized;
}

void Ball::Initialize(
    FPVector2D dimensions,
    FPVector3D location,
    FPVector2D startVector
    )
{
    // TODO: Time to factor out some mesh generation stuff :P

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

    const uint32_t NumVertices = sizeof(VertexPositions) / sizeof(VertexPositions[0]); 
    const uint32_t NumIndices = sizeof(VertexIndices) / sizeof(VertexIndices[0]); 

    auto pAssetStorageProvider = AssetStorageProvider::GetAssetStorageProviderAddress();
    auto pShaderStorage = pAssetStorageProvider->GetStorageByType<ShaderStorage>();
    ShaderPtr spShader = pShaderStorage->FindShaderByName(kBallShaderName);

    Mesh* pMeshComponent = GetComponentByType<Mesh>();
    RELEASE_CHECK(pMeshComponent, "Could not find mesh component on ball");

    pMeshComponent->SetPositions(VertexPositions, NumVertices);
    pMeshComponent->SetNormals(VertexNormals, NumVertices);
    pMeshComponent->SetTextureCoordinates(VertexUVs, NumVertices);
    pMeshComponent->SetIndices(VertexIndices, NumIndices);
    pMeshComponent->SetShader(spShader);
    pMeshComponent->SetModelMatrix(m_localTransform.ToMat4());

    static const glm::vec3 White(1.0, 1.0, 1.0);
    pMeshComponent->SetMeshUniforms({ MeshUniformValue("color", UniformType::Vec3, &White) });

    // Now, set up collision geometry! Defined in *global* space at the moment. TODO: Fix that
    // Because of this, gotta update geometry on tick... D: D:
    Collision* pCollision = GetComponentByType<Collision>();
    RELEASE_CHECK(pCollision, "Could not get collision component on Ball");

    pCollision->CollisionFlags = 0b111;
    pCollision->ObjectHandle = GetHandle();
    pCollision->NumCollisionShapes = 0;

    // Collides with everything
    const FPVector3D BallLowerLeft = FPVector3D(-dimensions.GetX() / FPTwo, -dimensions.GetY() / FPTwo, FPOne / FPTwo);
    pCollision->CollisionShapes[pCollision->NumCollisionShapes++] = AABB(
         BallLowerLeft + location,
         FPVector3D(dimensions.GetX(), dimensions.GetY(), FPOne)
         );

    Reset(location, startVector);
    m_isInitialized = true;
}

void Ball::Reset(
    sputter::math::FPVector3D location,
    sputter::math::FPVector2D startVector
    )
{
    // Relocate the ball
    const FPVector3D CurrentTranslation = m_localTransform.GetTranslation();
    TranslateBall(location - CurrentTranslation);

    // Set the initial motion vector
    if (startVector.IsZero())
    {
        m_travelVector = startVector;
    }
    else
    {
        m_travelVector = startVector.Normalized();
    }

    m_isAlive = true;

    Mesh* pMeshComponent = GetComponentByType<Mesh>();
    RELEASE_CHECK(pMeshComponent, "Could not find mesh component on ball");
    pMeshComponent->SetVisibility(true);
}

bool Ball::IsDead() const
{
    return !m_isAlive;
}

void Ball::SetCanCollideWithPaddle(uint32_t paddleIndex, bool canCollide)
{
    RELEASE_CHECK(paddleIndex == 0 || paddleIndex == 1, "Unexpected paddle index!");

    Collision* pCollision = GetComponentByType<Collision>();
    RELEASE_CHECK(pCollision, "Could not get collision component on Ball");

    const uint32_t BitFlags = 1 << paddleIndex;
    if (canCollide)
    {
        pCollision->CollisionFlags |= BitFlags;
    }
    else
    {
        pCollision->CollisionFlags &= ~BitFlags;
    }
}

void Ball::SetVelocity(const FPVector2D& velocity)
{
    if (velocity.IsZero())
    {
        m_travelVector = velocity;
    }
    else
    {
        // Set to constant speed for now
        m_travelVector = velocity.Normalized() * gameconstants::BallSpeed;
    }
}

FPVector3D Ball::GetPosition() const
{
    return m_localTransform.GetTranslation();
}

FPVector2D Ball::GetPosition2D() const
{
    const FPVector3D& Position = GetPosition();
    return FPVector2D(Position.GetX(), Position.GetY());
}

FPVector2D Ball::GetDimensions() const
{
    const FPVector3D& Scale = m_localTransform.GetScale();
    return FPVector2D(Scale.GetX(), Scale.GetY());
}

void Ball::TranslateBall(const FPVector3D& translation)
{
    const FPVector3D CurrentTranslation = m_localTransform.GetTranslation();
    m_localTransform.SetTranslation(CurrentTranslation + translation);

    Mesh* pMeshComponent = GetComponentByType<Mesh>();
    RELEASE_CHECK(pMeshComponent, "Could not find mesh component on ball");
    pMeshComponent->SetModelMatrix(m_localTransform.ToMat4());

    // Update collision transform as well
    const FPVector3D Scale = m_localTransform.GetScale();
    const FPVector3D BallLowerLeft = FPVector3D(-Scale.GetX() / FPTwo, -Scale.GetY() / FPTwo, FPOne / FPTwo);

    Collision* pCollision = GetComponentByType<Collision>();
    RELEASE_CHECK(pCollision, "Could not get collision component on Ball");
    AABB& myAABB = pCollision->CollisionShapes[pCollision->NumCollisionShapes - 1];
    myAABB.SetLowerLeft(BallLowerLeft + m_localTransform.GetTranslation());
}

const FPVector2D Ball::ComputeBallDeflectionFromPaddle(const Paddle* pPaddle) const
{
    // Get a vector from the a reference point on the paddle to the collision 
    // point, that's the travel direction.
    //
    // The "reference point" sits behind the paddle's midpoint at a distance from the 
    // paddle's edge defined by gameconstants::PaddleHitReferencePoint. The vector 
    // from that reference point to the ball's point of contact is the direction of 
    // deflection for the ball.
    const FixedPoint PaddleHalfWidth = pPaddle->GetDimensions().GetY() / FPTwo;
    const FPVector2D& MiddleBack = 
        pPaddle->GetPosition() - 
        (pPaddle->GetFacingDirection() * 
         (PaddleHalfWidth + gameconstants::PaddleHitReferencePointDistance));
    const FPVector2D PaddleReferencePoint = MiddleBack;

    if (debugsettings::GetDebugSetting("DrawPaddleDeflectionVector"))
    {
        // TODO: DebugDrawLine
        const float PreviousDepth = shapes::GetLineRendererDepth();
        shapes::SetLineRendererDepth(-1.f);

        const Vector2i RefPoint(
            (int)PaddleReferencePoint.GetX(),
            (int)PaddleReferencePoint.GetY());
        const Vector2i Pos((int)GetPosition().GetX(), (int)GetPosition().GetY());
        shapes::DrawLine(RefPoint, Pos, 5, Color::GREEN);

        shapes::SetLineRendererDepth(PreviousDepth);
    }

    return GetPosition2D() - PaddleReferencePoint;
}
