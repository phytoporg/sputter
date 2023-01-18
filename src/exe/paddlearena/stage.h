#pragma once

#include <string>
#include <memory>
#include <sputter/core/component.h>
#include <sputter/core/serializable.h>
#include <sputter/game/object.h>
#include <sputter/math/fpvector2d.h>
#include <sputter/math/fptransform3d.h>

namespace sputter {
    namespace render {
        class Mesh;
        class Shader;
    }

    namespace physics {
        struct Collision;
    }
}

class Stage : public sputter::game::Object, sputter::core::ISerializable
{
public:
    Stage();

    virtual void Tick(sputter::math::FixedPoint deltaTime) override;

    void Initialize(sputter::math::FPVector2D stageDimensions);

    // Begin ISerializable
    virtual bool Serialize(void* pBuffer, size_t* pBytesWrittenOut, size_t maxBytes) override;
    virtual bool Deserialize(void* pBuffer, size_t* pBytesReadOut, size_t maxBytes) override;
    // End ISerializable

private:
    static const std::string     kArenaVertexShaderAssetName;
    static const std::string     kArenaFragmentShaderAssetName;
    static const std::string     kArenaShaderName;

    sputter::math::FPTransform3D m_localTransform;
    sputter::math::FPVector3D    m_initialLocation;
};