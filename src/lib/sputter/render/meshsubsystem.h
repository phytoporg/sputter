#pragma once

#include "mesh.h"
#include <sputter/core/subsystem.h>
#include <sputter/containers/fixedmemoryvector.h>
#include <sputter/memory/fixedmemoryallocator.h>

namespace sputter { namespace render {
    struct MeshSubsystemSettings
    {
        size_t  MaxMeshCount = 1;
        size_t  MaxVertexCount = 1;
    };

    class MeshSubsystem : public core::ISubsystem<Mesh>
    {
    public:
        static const game::SubsystemType SubsystemId =
            game::SubsystemType::TYPE_MESH;

        MeshSubsystem(
            sputter::memory::FixedMemoryAllocator& allocator,
            const MeshSubsystemSettings& settings
            );

        virtual void Tick(math::FixedPoint dt) override;

        virtual Mesh* CreateComponent() override;
        virtual void ReleaseComponent(Mesh* pMesh) override;

        void Draw(const glm::mat4& projMatrix);

    private:
        MeshSubsystem() = delete;
        MeshSubsystem(const MeshSubsystem& other) = delete;

        sputter::memory::FixedMemoryAllocator&       m_allocator;

        size_t              m_maxMeshCount;
        size_t              m_maxVertexCount;

        sputter::containers::FixedMemoryVector<Mesh> m_meshes;

    };
}}