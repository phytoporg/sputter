#pragma once

#include "mesh.h"
#include <sputter/core/subsystem.h>
#include <sputter/core/component.h>
#include <sputter/game/subsystemtype.h>
#include <sputter/containers/fixedmemoryvector.h>
#include <sputter/memory/fixedmemoryallocator.h>

#include <vector>
#include <glm/glm.hpp>

namespace sputter { namespace render {
    struct MeshSubsystemSettings
    {
        size_t  MaxMeshCount = 1;
        size_t  MaxVertexCount = 1;
        size_t  MaxIndexCount = 1;
    };

    class MeshSubsystem : public core::ISubsystem<Mesh>
    {
    public:
        static const sputter::game::SubsystemType SubsystemId =
            sputter::game::SubsystemType::TYPE_MESH;

        MeshSubsystem(const MeshSubsystemSettings& settings);

        virtual void Tick(math::FixedPoint dt) override;

        virtual Mesh* CreateComponent(const Mesh::InitializationParameters& params) override;
        virtual void ReleaseComponent(Mesh* pMesh) override;

        core::ComponentHandle GetComponentHandle(Mesh* pMesh) const;
        Mesh* GetComponentFromHandle(core::ComponentHandle handle);

        void Draw(const glm::mat4& projMatrix, const glm::mat4& viewMatrix);

    private:
        MeshSubsystem() = delete;
        MeshSubsystem(const MeshSubsystem& other) = delete;

        size_t              m_maxMeshCount;
        size_t              m_maxVertexCount;
        size_t              m_maxIndexCount;

        std::vector<Mesh>   m_meshes;
    };
}}