#include "ball.h"
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

#include <sputter/physics/aabb.h>
#include <sputter/physics/collision.h>
#include <sputter/physics/collisionsubsystem.h>

#include <fpm/math.hpp>

using namespace sputter::render;
using namespace sputter::game;
using namespace sputter::assets;
using namespace sputter::math;
using namespace sputter::physics;

// Same ol' same ol' for now
const std::string Ball::kBallVertexShaderAssetName = "cube_vert";
const std::string Ball::kBallFragmentShaderAssetName = "cube_frag";
const std::string Ball::kBallShaderName = "cube_shader";

Ball::Ball(
    AssetStorageProvider* pStorageProvider,
    SubsystemProvider* pSubsystemProvider
) : Object(kPaddleArenaObjectTypeBall, pStorageProvider, pSubsystemProvider)
{
    {
        sputter::render::Mesh::InitializationParameters params = {};
        CreateAndSetComponentByType<MeshSubsystem>(&m_pMeshComponent, params);
        if (!m_pMeshComponent)
        {
            sputter::system::LogAndFail("Failed to create mesh component in Ball object.");
        }
    }

    {
        sputter::physics::Collision::InitializationParameters params = {};
        CreateAndSetComponentByType<CollisionSubsystem>(&m_pCollisionComponent, params);
        if (!m_pCollisionComponent)
        {
            sputter::system::LogAndFail("Failed to create collision component in Ball object.");
        }
    }

    auto pShaderStorage = pStorageProvider->GetStorageByType<ShaderStorage>();
    m_spShader = pShaderStorage->FindShaderByName(kBallShaderName);
    if (!m_spShader)
    {
        if (!pShaderStorage->AddShaderFromShaderAssetNames(
            pStorageProvider->GetGeneralStorage(),
            kBallVertexShaderAssetName,
            kBallFragmentShaderAssetName,
            kBallShaderName))
        {
            sputter::system::LogAndFail("Failed to add shader for ball.");
        }
    }

    m_spShader = pShaderStorage->FindShaderByName(kBallShaderName);
    if (!m_spShader)
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

    const FixedPoint Speed = kGameConstantsBallSpeed;
    if (m_travelVector.Length() > FPZero)
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
                m_pMeshComponent->SetVisibility(false);
            }
        }
        else if (OtherCollision.pObject->GetType() == kPaddleArenaObjectTypePaddle)
        {
            const ICollisionShape* pOtherShape = collisionResult.pCollisionA == m_pCollisionComponent ?
                collisionResult.pCollisionShapeB : collisionResult.pCollisionShapeA;

            // lol this is also hideous
            AABB* pMyAABB = static_cast<AABB*>(m_pCollisionComponent->CollisionShapes.back());
            const AABB* pOtherAABB = static_cast<const AABB*>(pOtherShape);
            const FPVector3D Separation = pMyAABB->GetSeparation2D(pOtherAABB);

            if (fpm::abs(Separation.GetX()) > FPZero)
            {
                // Colliding with the side of the paddle. Negate x-axis travel direction and 
                // correct separation.
                m_travelVector.SetX(-m_travelVector.GetX());
                TranslateBall(-Separation * (FPOne + FPEpsilon));
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
    m_pCollisionComponent->CollisionFlags = 0x111;
    m_pCollisionComponent->pObject = this;
    m_pCollisionComponent->CollisionShapes.clear();

    // Collides with everything
    const FPVector3D BallLowerLeft = FPVector3D(-dimensions.GetX() / FPTwo, -dimensions.GetY() / FPTwo, FPOne / FPTwo);
    AABB* pShape = new AABB(
         BallLowerLeft + location,
         FPVector3D(dimensions.GetX(), dimensions.GetY(), FPOne)
         );
    m_pCollisionComponent->CollisionShapes.push_back(pShape);

    Reset(location, startVector);
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
    m_travelVector = startVector.Normalized();
    m_isAlive = true;
    m_pMeshComponent->SetVisibility(true);
}

bool Ball::IsDead() const
{
    return !m_isAlive;
}

FPVector3D Ball::GetPosition() const
{
    return m_localTransform.GetTranslation();
}

void Ball::TranslateBall(const FPVector3D& translation)
{
    const FPVector3D CurrentTranslation = m_localTransform.GetTranslation();
    m_localTransform.SetTranslation(CurrentTranslation + translation);
    m_pMeshComponent->SetModelMatrix(m_localTransform.ToMat4());

    // Update collision transform as well
    const FPVector3D Scale = m_localTransform.GetScale();
    const FPVector3D BallLowerLeft = FPVector3D(-Scale.GetX() / FPTwo, -Scale.GetY() / FPTwo, FPOne / FPTwo);

    AABB* pMyAABB = static_cast<AABB*>(m_pCollisionComponent->CollisionShapes.back());
    pMyAABB->SetLowerLeft(BallLowerLeft + m_localTransform.GetTranslation());
}