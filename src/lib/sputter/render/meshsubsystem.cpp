#include "meshsubsystem.h"

#include <sputter/core/check.h>

using namespace sputter;
using namespace sputter::render;

MeshSubsystem::MeshSubsystem(const MeshSubsystemSettings& /*settings*/)
{}

void MeshSubsystem::Tick(math::FixedPoint dt) 
{
    // NOOP for now
}

Mesh* MeshSubsystem::CreateComponent(const Mesh::InitializationParameters& /*params*/) 
{
    if (m_meshCount >= kMaxMeshes)
    {
        return nullptr;
    }

    // Find the first available slot
    Mesh** ppMesh = &m_pMeshes[0];
    while(*ppMesh != nullptr)
    {
        ++ppMesh;
    }

    RELEASE_CHECK(*ppMesh == nullptr, "Could not find available mesh slot");
    // TODO: Use a fixed memory allocator?
    *ppMesh = new Mesh(kMaxVertices, kMaxIndices);
    RELEASE_CHECK(*ppMesh != nullptr, "Failed to allocate new mesh");

    ++m_meshCount;
    return *ppMesh;
}

void MeshSubsystem::ReleaseComponent(Mesh* pMesh) 
{
    static const size_t InvalidIndex = static_cast<size_t>(-1);
    auto indexToRemove = InvalidIndex;
    for (size_t i = 0; i < kMaxMeshes; ++i)
    {
        if (m_pMeshes[i] == pMesh)
        {
            indexToRemove = i;
            break;
        }
    }

    if (indexToRemove != InvalidIndex)
    {
        delete m_pMeshes[indexToRemove];
        m_pMeshes[indexToRemove] = nullptr;
        m_meshCount--;
    }
}

core::ComponentHandle MeshSubsystem::GetComponentHandle(Mesh* pMesh) const
{
    for (uint16_t i = 0; i < kMaxMeshes; ++i)
    {
        if (pMesh == m_pMeshes[i])
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
    return m_pMeshes[Index];
}

void MeshSubsystem::Draw(const glm::mat4& projMatrix, const glm::mat4& viewMatrix) 
{
    for (size_t i = 0; i < m_meshCount; i++)
    {
        if (m_pMeshes[i])
        {
            m_pMeshes[i]->Draw(projMatrix, viewMatrix);
        }
    }
}
