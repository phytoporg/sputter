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
#include <sputter/math/fpconstants.h>

#include <sputter/log/framestatelogger.h>

namespace sputter
{
    namespace render
    {
        class Mesh;
        class Shader;
        class MeshSubsystem;
    }

    namespace input
    {
        class InputSource;
    }

    namespace physics
    {
        struct Collision;
    }
}

struct GameState;
class Ball;
class Paddle : public sputter::game::Object, public sputter::core::ISerializable
{
public:
    Paddle(uint32_t playerId);

    virtual void Tick(sputter::math::FixedPoint deltaTime) override;
    virtual void PostTick(sputter::math::FixedPoint deltaTime) override;

    // Begin ISerializable
    virtual bool Serialize(void* pBuffer, size_t* pBytesWrittenOut, size_t maxBytes) override;
    virtual bool Deserialize(void* pBuffer, size_t* pBytesReadOut, size_t maxBytes) override;
    virtual const char * GetName() const override { return "Paddle"; };
    // End ISerializable

    void Initialize(
        sputter::math::FPVector2D dimensions,
        sputter::math::FPVector3D location
        );

    void AttachBall(Ball* pBall);
    void DetachBall(const sputter::math::FPVector2D& detachVelocity);
    bool IsBallAttached() const;

    sputter::math::FPVector2D GetFacingDirection() const;

    sputter::math::FPVector2D GetPosition() const;
    sputter::math::FPVector2D GetDimensions() const;

    uint32_t GetPlayerID() const;

private:
    // Some private helpers
    void TranslatePaddle(const sputter::math::FPVector3D& translation);

private:
    static const std::string       kPaddleVertexShaderAssetName;
    static const std::string       kPaddleFragmentShaderAssetName;
    static const std::string       kPaddleShaderName;

    sputter::assets::ResourceHandle          m_shaderHandle = sputter::assets::kInvalidResourceHandle;

    sputter::math::FPTransform3D             m_localTransform;
    sputter::math::FPVector3D                m_initialLocation;

    uint32_t                                 m_playerId;
    sputter::math::FixedPoint                m_dashVelocityY = sputter::math::FPZero;

    bool                                     m_ballAttached = false;
};
