#include "mesh.h"
#include "attribute.h"
#include "indexbuffer.h"

#include <vector>
#include <glm/glm.hpp>

#include <sputter/system/system.h>

using namespace sputter::render;
using namespace sputter::containers;
using namespace sputter::math;

struct Mesh::PImpl
{
    // Attributes & EOB
    // Is this really needed per-mesh?
    Attribute<glm::vec3> VertexPositionAttribute;
    Attribute<glm::vec3> VertexNormalAttribute;
    Attribute<glm::vec2> VertexTextureCoordinateAttribute;
    IndexBuffer          IndexBuffer;

    // Data
    std::vector<glm::vec3> VertexPositions;
    std::vector<glm::vec3> VertexNormals;
    std::vector<glm::vec2> VertexTextureCoordinates;
    std::vector<int>       VertexIndices;

    ShaderPtr              spShader;

    void CopyTo(Mesh::PImpl& other)
    {
        VertexPositionAttribute = other.VertexPositionAttribute;
        VertexNormalAttribute = other.VertexNormalAttribute;
        VertexTextureCoordinateAttribute = other.VertexTextureCoordinateAttribute;
        IndexBuffer = other.IndexBuffer;

        VertexPositions = other.VertexPositions;
        VertexNormals = other.VertexNormals;
        VertexTextureCoordinates = other.VertexTextureCoordinates;
        VertexIndices = other.VertexIndices;

        spShader = other.spShader;
    }

    void BindAttributes()
    {
        VertexPositionAttribute.BindTo(0);
        VertexNormalAttribute.BindTo(1);
        VertexTextureCoordinateAttribute.BindTo(2);
    }
};

Mesh::Mesh() 
    : m_spPimpl(new Mesh::PImpl)
{}

Mesh::Mesh(const Mesh& other)
{
    // Deep copy
    m_spPimpl = std::make_shared<Mesh::PImpl>();
    other.m_spPimpl->CopyTo(*m_spPimpl);
}

Mesh& Mesh::operator=(const Mesh& other)
{
    m_spPimpl = std::make_shared<Mesh::PImpl>();
    other.m_spPimpl->CopyTo(*m_spPimpl);
    return *this;
}

bool Mesh::SetPositions(
    const FixedMemoryVector<FPVector3D>& vertexPositions
    )
{
    return SetPositions(vertexPositions.Data(), vertexPositions.Size());
}

bool Mesh::SetPositions(
    const FPVector3D* positionsArray, uint32_t arrayLen
    )
{
    // Convert to floating-point representation for GPU
    m_spPimpl->VertexPositions.clear();
    m_spPimpl->VertexPositions.reserve(arrayLen);
    for (uint32_t i = 0; i < arrayLen; ++i)
    {
        m_spPimpl->VertexPositions.emplace_back(positionsArray[i].ToVec3());
    }

    m_spPimpl->VertexPositionAttribute.Set(m_spPimpl->VertexPositions);
    return true;
}

bool Mesh::SetNormals(
    const FixedMemoryVector<FPVector3D>& vertexNormals
    )
{
    return SetNormals(vertexNormals.Data(), vertexNormals.Size());
}

bool Mesh::SetNormals(
    const FPVector3D* normalsArray, uint32_t arrayLen
    )
{
    // Convert to floating-point representation for GPU
    m_spPimpl->VertexNormals.clear();
    m_spPimpl->VertexNormals.reserve(arrayLen);
    for (size_t i = 0; i < arrayLen; ++i)
    {
        m_spPimpl->VertexNormals.emplace_back(normalsArray[i].ToVec3());
    }

    m_spPimpl->VertexNormalAttribute.Set(m_spPimpl->VertexNormals);
    return true;
}

bool Mesh::SetTextureCoordinates(const FixedMemoryVector<FPVector2D>& vertexUVs)
{
    return SetTextureCoordinates(vertexUVs.Data(), vertexUVs.Size());
}

bool Mesh::SetTextureCoordinates(const FPVector2D* uvsArray, uint32_t arrayLen)
{
    // Convert to floating-point representation for GPU
    m_spPimpl->VertexTextureCoordinates.clear();
    m_spPimpl->VertexTextureCoordinates.reserve(arrayLen);
    for (size_t i = 0; i < arrayLen; ++i)
    {
        m_spPimpl->VertexTextureCoordinates.emplace_back(uvsArray[i].ToVec2());
    }

    m_spPimpl->VertexTextureCoordinateAttribute.Set(m_spPimpl->VertexTextureCoordinates);
    return true;
}

bool Mesh::SetIndices(
    const FixedMemoryVector<int>& indices
    )
{
    return SetIndices(indices.Data(), indices.Size());
}

bool Mesh::SetIndices(
    const int* indexArray, uint32_t arrayLen
    )
{    
    // No *real* need for conversion here, but for the sake of consistency...
    m_spPimpl->VertexIndices.clear();
    m_spPimpl->VertexIndices.resize(arrayLen);

    memcpy(m_spPimpl->VertexIndices.data(), indexArray, arrayLen * sizeof(int));
    m_spPimpl->IndexBuffer.Set(m_spPimpl->VertexIndices);
    return true;
}

bool Mesh::SetShader(ShaderPtr spShader)
{
    if (!spShader)
    {
        return false;
    }
    
    m_spPimpl->spShader = spShader;
    return true;
}

void Mesh::Draw(const glm::mat4& projMatrix) 
{

}
