#include "mesh.h"
#include "render.h"
#include "uniform.h"
#include "attribute.h"
#include "indexbuffer.h"
#include "draw.h"

#include <vector>
#include <glm/glm.hpp>

#include <sputter/render/uniform.h>

#include <sputter/system/system.h>

using namespace sputter::render;
using namespace sputter::containers;
using namespace sputter::math;

struct Mesh::PImpl
{
    // Attributes & EOB
    // Is this really needed per-mesh?
    Attribute<glm::vec3>           VertexPositionAttribute;
    Attribute<glm::vec3>           VertexNormalAttribute;
    Attribute<glm::vec2>           VertexTextureCoordinateAttribute;
    IndexBuffer                    Indices;

    // Data
    std::vector<glm::vec3>         VertexPositions;
    std::vector<glm::vec3>         VertexNormals;
    std::vector<glm::vec2>         VertexTextureCoordinates;
    std::vector<uint32_t>          VertexIndices;

    std::vector<MeshUniformValue>  MeshUniformValues;

    glm::mat4              ModelMatrix;

    ShaderPtr              spShader;

    uint32_t               VAO;

    uint32_t               ModelUniformHandle;
    uint32_t               ViewUniformHandle;
    uint32_t               ProjUniformHandle;

    bool                   IsDirty   = true;
    bool                   IsVisible = true;

    void CopyTo(Mesh::PImpl& other)
    {
        VertexPositionAttribute = other.VertexPositionAttribute;
        VertexNormalAttribute = other.VertexNormalAttribute;
        VertexTextureCoordinateAttribute = other.VertexTextureCoordinateAttribute;
        Indices = other.Indices;

        VertexPositions = other.VertexPositions;
        VertexNormals = other.VertexNormals;
        VertexTextureCoordinates = other.VertexTextureCoordinates;
        VertexIndices = other.VertexIndices;

        MeshUniformValues = other.MeshUniformValues;

        spShader = other.spShader;

        VAO = other.VAO;

        ModelUniformHandle = other.ModelUniformHandle;
        ViewUniformHandle = other.ViewUniformHandle;
        ProjUniformHandle = other.ProjUniformHandle;

        IsVisible = other.IsVisible;

        IsDirty   = true;
    }

    void BindAttributes()
    {
        VertexPositionAttribute.BindTo(0);
        VertexNormalAttribute.BindTo(1);
        VertexTextureCoordinateAttribute.BindTo(2);
    }

    void UnbindAttributes()
    {
        VertexPositionAttribute.UnbindFrom(0);
        VertexNormalAttribute.UnbindFrom(1);
        VertexTextureCoordinateAttribute.UnbindFrom(2);
    }
};

Mesh::Mesh(size_t maxVertexCount, size_t maxIndexCount) 
{
    uint32_t vao;
    glGenVertexArrays(1, &vao);

    m_spPimpl = std::make_shared<Mesh::PImpl>();
    m_spPimpl->VertexPositions.reserve(maxVertexCount);
    m_spPimpl->VertexPositionAttribute.Set(m_spPimpl->VertexPositions.data(), maxVertexCount);
    m_spPimpl->VertexPositionAttribute.BindTo(0);

    m_spPimpl->VertexNormals.reserve(maxVertexCount);
    m_spPimpl->VertexNormalAttribute.Set(m_spPimpl->VertexNormals.data(), maxVertexCount);
    m_spPimpl->VertexNormalAttribute.BindTo(1);

    m_spPimpl->VertexTextureCoordinates.reserve(maxVertexCount);
    m_spPimpl->VertexTextureCoordinateAttribute.Set(m_spPimpl->VertexTextureCoordinates.data(), maxVertexCount);
    m_spPimpl->VertexTextureCoordinateAttribute.BindTo(2);

    m_spPimpl->VertexIndices.reserve(maxIndexCount);
    m_spPimpl->Indices.Set(m_spPimpl->VertexIndices.data(), maxIndexCount);

    m_spPimpl->VAO = vao;

    glBindVertexArray(0);
}

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
    m_spPimpl->IsDirty = true;
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
    for (uint32_t i = 0; i < arrayLen; ++i)
    {
        m_spPimpl->VertexPositions.emplace_back(positionsArray[i].ToVec3());
    }

    m_spPimpl->VertexPositionAttribute.Set(m_spPimpl->VertexPositions);
    m_spPimpl->IsDirty = true;
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
    m_spPimpl->IsDirty = true;
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
    m_spPimpl->IsDirty = true;
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
    m_spPimpl->Indices.Set(m_spPimpl->VertexIndices);
    m_spPimpl->IsDirty = true;
    return true;
}

bool Mesh::SetShader(ShaderPtr spShader)
{
    if (!spShader)
    {
        return false;
    }
    
    m_spPimpl->spShader = spShader;
    m_spPimpl->ModelUniformHandle = spShader->GetUniform("model");
    if (m_spPimpl->ModelUniformHandle == Shader::kInvalidHandleValue)
    {
        LOG(ERROR) << "Could not retrieve expected uniform 'model' from shader '" 
                   << spShader->GetName() << "'";
        return false;
    }

    m_spPimpl->ViewUniformHandle = spShader->GetUniform("view");
    if (m_spPimpl->ViewUniformHandle == Shader::kInvalidHandleValue)
    {
        LOG(ERROR) << "Could not retrieve expected uniform 'view' from shader '" 
                   << spShader->GetName() << "'";
        return false;
    }

    m_spPimpl->ProjUniformHandle = spShader->GetUniform("projection");
    if (m_spPimpl->ProjUniformHandle == Shader::kInvalidHandleValue)
    {
        LOG(ERROR) << "Could not retrieve expected uniform 'projection' from shader '" 
                   << spShader->GetName() << "'";
        return false;
    }

    return true;
}

void Mesh::SetModelMatrix(const glm::mat4& modelMatrix) 
{
    m_spPimpl->ModelMatrix = modelMatrix;
}

void Mesh::SetMeshUniforms(const std::vector<MeshUniformValue>& uniformValues)
{
    m_spPimpl->MeshUniformValues = uniformValues;
}

void Mesh::Draw(const glm::mat4& projMatrix, const glm::mat4& viewMatrix) 
{
    if (!GetVisibility())
    {
        // Nothing to do if we're not visible
        return;
    }
    

    if (!m_spPimpl->spShader || 
         m_spPimpl->ModelUniformHandle == Shader::kInvalidHandleValue ||
         m_spPimpl->ViewUniformHandle  == Shader::kInvalidHandleValue ||
         m_spPimpl->ProjUniformHandle  == Shader::kInvalidHandleValue)
    {
        LOG(WARNING) << "Attempting to render mesh with an improper or invalid shader.";
        return;
    }

    m_spPimpl->spShader->Bind();

    for (MeshUniformValue& uniformValue : m_spPimpl->MeshUniformValues)
    {
        if (uniformValue.Location == Shader::kInvalidHandleValue)
        {
            uniformValue.Location = m_spPimpl->spShader->GetUniform(uniformValue.Name);
        }

        if (uniformValue.Location != Shader::kInvalidHandleValue)
        {
            SetUniformByType(uniformValue.Location, uniformValue.Type, uniformValue.pValue);
        }
        else
        {
            // Log something if we didn't get a valid response in the lookup
            LOG(ERROR) << "Failed to retrieve a location for uniform named '" << uniformValue.Name 
                       << "' in shader '" << m_spPimpl->spShader->GetName() << "'";
        }
    }

    Uniform<glm::mat4>::Set(m_spPimpl->ModelUniformHandle, m_spPimpl->ModelMatrix);
    Uniform<glm::mat4>::Set(m_spPimpl->ViewUniformHandle, viewMatrix);
    Uniform<glm::mat4>::Set(m_spPimpl->ProjUniformHandle, projMatrix);

    glBindVertexArray(m_spPimpl->VAO);

    m_spPimpl->BindAttributes();

    if (m_spPimpl->IsDirty)
    {
        // Update buffer objects
        m_spPimpl->VertexPositionAttribute.Set(m_spPimpl->VertexPositions);
        m_spPimpl->VertexNormalAttribute.Set(m_spPimpl->VertexNormals);
        m_spPimpl->VertexTextureCoordinateAttribute.Set(m_spPimpl->VertexTextureCoordinates);
        m_spPimpl->Indices.Set(m_spPimpl->VertexIndices);
        m_spPimpl->IsDirty = false;
    }

    ::Draw(m_spPimpl->Indices, DrawMode::Triangles);
    m_spPimpl->UnbindAttributes();

    m_spPimpl->spShader->Unbind();

    glBindVertexArray(0);
}

void Mesh::SetVisibility(bool newVisibility)
{
    m_spPimpl->IsVisible = newVisibility;
}

bool Mesh::GetVisibility() const
{
    return m_spPimpl->IsVisible;
}

