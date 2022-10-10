#include "meshsubsystem.h"

#include <sputter/core/check.h>

using namespace sputter;
using namespace sputter::render;

MeshSubsystem::MeshSubsystem(
    const MeshSubsystemSettings& settings
    ) : m_maxMeshCount(settings.MaxMeshCount), 
        m_maxVertexCount(settings.MaxVertexCount),
        m_maxIndexCount(settings.MaxIndexCount)
{
    m_meshes.reserve(m_maxMeshCount);
}

void MeshSubsystem::Tick(math::FixedPoint dt) 
{
    // NOOP for now
}

Mesh* MeshSubsystem::CreateComponent(const Mesh::InitializationParameters& /*params*/) 
{
    m_meshes.emplace_back(m_maxVertexCount, m_maxIndexCount);
    return &m_meshes.back();
}

void MeshSubsystem::ReleaseComponent(Mesh* pMesh) 
{
    static const size_t InvalidIndex = static_cast<size_t>(-1);
    auto indexToRemove = InvalidIndex;
    for (size_t i = 0; i < m_meshes.size(); ++i)
    {
        if (&(m_meshes[i]) == pMesh)
        {
            indexToRemove = i;
            break;
        }
    }

    if (indexToRemove != InvalidIndex)
    {
        m_meshes.erase(std::begin(m_meshes) + indexToRemove);
    }
}

core::ComponentHandle MeshSubsystem::GetComponentHandle(Mesh* pMesh) const
{
    for (uint16_t i = 0; i < m_meshes.size(); ++i)
    {
        if (pMesh == &m_meshes[i])
        {
            return core::CreateComponentHandle<Mesh>(i);
        }
    }

    return core::kInvalidComponentHandle;
}

Mesh* MeshSubsystem::GetComponentFromHandle(core::ComponentHandle handle)
{
    RELEASE_CHECK(handle != core::kInvalidComponentHandle, "Invalid mesh handle");
    const uint16_t Index = core::GetComponentIndexFromHandle(handle);
    return &m_meshes[Index];
}

void MeshSubsystem::Draw(const glm::mat4& projMatrix, const glm::mat4& viewMatrix) 
{
    for (Mesh& mesh : m_meshes)
    {
        mesh.Draw(projMatrix, viewMatrix);
    }
}
