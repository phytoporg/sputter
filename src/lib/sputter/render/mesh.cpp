#include "mesh.h"
#include <sputter/system/system.h>

using namespace sputter::render;
using namespace sputter::containers;
using namespace sputter::math;

Mesh::Mesh(
    uint32_t maxVertices,
    ) : m_maxVertices(maxVertices)
{}

bool Mesh::SetVertices(
    const FixedMemoryVector<FPVector3D>& vertices
    )
{
    if (vertices.Size() > m_maxVertices)
    {
        return false;
    }

    m_vertices = vertices;
    return true;
}

bool Mesh::SetIndices(
    const FixedMemoryVector<int>& indices
    )
{
#if defined(DEBUG)
    for (size_t i = 0; i < indices.Size(); ++i)
    {
        if (indices[i] >= m_maxVertices)
        {
            sputter::system::LogAndFail("Index out of bounds in mesh indices.");
        }
    }
#endif

    m_indices = indices;
    return true;
}

bool Mesh::SetShader(ShaderPtr spShader)
{
    m_spShader = spShader;
}