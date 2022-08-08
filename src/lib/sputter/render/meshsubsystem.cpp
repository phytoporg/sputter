#include "meshsubsystem.h"

using namespace sputter::render;

MeshSubsystem::MeshSubsystem(
    memory::FixedMemoryAllocator& allocator
    const MeshSubsystemSettings& settings
    ) : m_meshCount(0),
        m_maxMeshCount(settings.MaxMeshCount), 
        m_maxVertexCount(settings.MaxVertexCount), 
        m_allocator(allocator),
        m_meshes(m_maxMeshCount, allocator)
{}

void MeshSubsystem::Tick(math::FixedPoint dt) 
{
    for (size_t i = 0; i < m_meshes.size(); ++i)
    {
        m_meshes[i].Tick(dt);
    }
}

Mesh* MeshSubsystem::CreateComponent() 
{
    m_meshes.Emplace(m_maxVertexCount);
    return &m_meshes.Back();
}

void MeshSubsystem::ReleaseComponent(Mesh* pMesh) 
{
    static const size_t InvalidIndex = static_cast<size_t>(-1);
    auto indexToRemove = InvalidIndex;
    for (size_t i = 0; i < m_meshes.size(); ++i)
    {
        if (m_meshes[i] == pMesh)
        {
            indexToRemove = i;
            break;
        }
    }

    if (indexToRemove != InvalidIndex)
    {
        m_meshes.Remove(indexToRemove);
    }
}

void MeshSubsystem::Draw(const glm::mat4& projMatrix) 
{
    // TODO!
}
