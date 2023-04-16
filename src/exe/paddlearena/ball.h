#pragma once

#include <string>
#include <memory>

#include <sputter/assets/resourcestorage.h>
#include <sputter/core/component.h>
#include <sputter/core/serializable.h>
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

class Paddle;
class Ball : public sputter::game::Object, public sputter::core::ISerializable
{
public:
    Ball();

    virtual void Tick(sputter::math::FixedPoint deltaTime) override;
    virtual void PostTick(sputter::math::FixedPoint deltaTime) override;
    virtual const char * GetName() const override { return "Ball"; };

    // Begin ISerializable
    virtual bool Serialize(void* pBuffer, size_t* pBytesWrittenOut, size_t maxBytes) override;
    virtual bool Deserialize(void* pBuffer, size_t* pBytesReadOut, size_t maxBytes) override;
    // End ISerializable

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
    sputter::math::FPVector2D GetPosition2D() const;
    sputter::math::FPVector2D GetDimensions() const;

    void TranslateBall(const sputter::math::FPVector3D& translation);

private:
    const sputter::math::FPVector2D ComputeBallDeflectionFromPaddle(const Paddle* pPaddle) const;

    static const std::string       kBallVertexShaderAssetName;
    static const std::string       kBallFragmentShaderAssetName;
    static const std::string       kBallShaderName;

    sputter::math::FPTransform3D   m_localTransform;
    sputter::math::FPVector3D      m_initialLocation;
    sputter::math::FPVector2D      m_travelVector;

    bool                           m_isAlive = false;
    bool                           m_isInitialized = false;
};
