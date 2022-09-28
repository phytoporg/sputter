#pragma once

#include <string>
#include <memory>

#include <sputter/game/object.h>
#include <sputter/math/fixedpoint.h>
#include <sputter/math/fptransform3d.h>
#include <sputter/math/fpvector2d.h>
#include <sputter/math/fpvector3d.h>

namespace sputter
{
    namespace render
    {
        class Mesh;
        class Shader;
        class MeshSubsystem;
    }

    namespace physics
    {
        struct Collision;
    }
}

class Ball : sputter::game::Object
{
public:
    Ball(
        sputter::assets::AssetStorageProvider* pStorageProvider,
        sputter::game::SubsystemProvider* pSubsystemProvider
        );

    virtual void Tick(sputter::math::FixedPoint deltaTime) override;
    virtual void PostTick(sputter::math::FixedPoint deltaTime) override;

    bool IsInitialized() const;
    void Initialize(
        sputter::math::FPVector2D dimensions,
        sputter::math::FPVector3D location,
        sputter::math::FPVector2D startVector
        );
    void Reset(
        sputter::math::FPVector3D location,
        sputter::math::FPVector2D startVector
        );

    bool IsDead() const;

    void SetCanCollideWithPaddle(uint32_t paddleIndex, bool canCollide);
    void SetVelocity(const sputter::math::FPVector2D& velocity);

    sputter::math::FPVector3D GetPosition() const;
    sputter::math::FPVector2D GetDimensions() const;

    void TranslateBall(const sputter::math::FPVector3D& translation);

private:
    static const std::string       kBallVertexShaderAssetName;
    static const std::string       kBallFragmentShaderAssetName;
    static const std::string       kBallShaderName;

    sputter::render::Mesh*                   m_pMeshComponent = nullptr;
    std::shared_ptr<sputter::render::Shader> m_spShader = nullptr;
    sputter::physics::Collision*             m_pCollisionComponent = nullptr;

    sputter::math::FPTransform3D             m_localTransform;
    sputter::math::FPVector3D                m_initialLocation;
    sputter::math::FPVector2D                m_travelVector;

    bool                                     m_isAlive = false;
    bool                                     m_isInitialized = false;
};