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

void Mesh::CopyTo(Mesh& other) const
{
    other.m_VertexPositionAttribute = m_VertexPositionAttribute;
    other.m_VertexNormalAttribute = m_VertexNormalAttribute;
    other.m_VertexTextureCoordinateAttribute = m_VertexTextureCoordinateAttribute;
    other.m_Indices = m_Indices;

    other.m_VertexPositions = m_VertexPositions;
    other.m_VertexNormals = m_VertexNormals;
    other.m_VertexTextureCoordinates = m_VertexTextureCoordinates;
    other.m_VertexIndices = other.m_VertexIndices;

    other.m_MeshUniformValues = m_MeshUniformValues;

    other.m_spShader = m_spShader;

    other.m_VAO = m_VAO;

    other.m_ModelUniformHandle = m_ModelUniformHandle;
    other.m_ViewUniformHandle = m_ViewUniformHandle;
    other.m_ProjUniformHandle = m_ProjUniformHandle;

    other.m_isVisible = m_isVisible;

    other.m_isDirty   = true;
}

void Mesh::BindAttributes()
{
    m_VertexPositionAttribute.BindTo(0);
    m_VertexNormalAttribute.BindTo(1);
    m_VertexTextureCoordinateAttribute.BindTo(2);
}

void Mesh::UnbindAttributes()
{
    m_VertexPositionAttribute.UnbindFrom(0);
    m_VertexNormalAttribute.UnbindFrom(1);
    m_VertexTextureCoordinateAttribute.UnbindFrom(2);
}

Mesh::Mesh(size_t maxVertexCount, size_t maxIndexCount) 
{
    uint32_t vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    m_VertexPositions.reserve(maxVertexCount);
    m_VertexPositionAttribute.Set(m_VertexPositions.data(), maxVertexCount);
    m_VertexPositionAttribute.BindTo(0);

    m_VertexNormals.reserve(maxVertexCount);
    m_VertexNormalAttribute.Set(m_VertexNormals.data(), maxVertexCount);
    m_VertexNormalAttribute.BindTo(1);

    m_VertexTextureCoordinates.reserve(maxVertexCount);
    m_VertexTextureCoordinateAttribute.Set(m_VertexTextureCoordinates.data(), maxVertexCount);
    m_VertexTextureCoordinateAttribute.BindTo(2);

    m_VertexIndices.reserve(maxIndexCount);
    m_Indices.Set(m_VertexIndices.data(), maxIndexCount);

    m_VAO = vao;

    glBindVertexArray(0);
}

Mesh::Mesh(const Mesh& other)
{
    // Deep copy
    other.CopyTo(*this);
}

Mesh& Mesh::operator=(const Mesh& other)
{
    other.CopyTo(*this);
    m_isDirty = true;
    return *this;
}

bool Mesh::SetPositions(const FixedMemoryVector<FPVector3D>& vertexPositions)
{
    return SetPositions(vertexPositions.Data(), vertexPositions.Size());
}

bool Mesh::SetPositions(const FPVector3D* positionsArray, uint32_t arrayLen)
{
    // Convert to floating-point representation for GPU
    for (uint32_t i = 0; i < arrayLen; ++i)
    {
        m_VertexPositions.emplace_back(positionsArray[i].ToVec3());
    }

    m_VertexPositionAttribute.Set(m_VertexPositions);
    m_isDirty = true;
    return true;
}

bool Mesh::SetNormals(const FixedMemoryVector<FPVector3D>& vertexNormals)
{
    return SetNormals(vertexNormals.Data(), vertexNormals.Size());
}

bool Mesh::SetNormals(const FPVector3D* normalsArray, uint32_t arrayLen)
{
    // Convert to floating-point representation for GPU
    m_VertexNormals.clear();
    m_VertexNormals.reserve(arrayLen);
    for (size_t i = 0; i < arrayLen; ++i)
    {
        m_VertexNormals.emplace_back(normalsArray[i].ToVec3());
    }

    m_VertexNormalAttribute.Set(m_VertexNormals);
    m_isDirty = true;
    return true;
}

bool Mesh::SetTextureCoordinates(const FixedMemoryVector<FPVector2D>& vertexUVs)
{
    return SetTextureCoordinates(vertexUVs.Data(), vertexUVs.Size());
}

bool Mesh::SetTextureCoordinates(const FPVector2D* uvsArray, uint32_t arrayLen)
{
    // Convert to floating-point representation for GPU
    m_VertexTextureCoordinates.clear();
    m_VertexTextureCoordinates.reserve(arrayLen);
    for (size_t i = 0; i < arrayLen; ++i)
    {
        m_VertexTextureCoordinates.emplace_back(uvsArray[i].ToVec2());
    }

    m_VertexTextureCoordinateAttribute.Set(m_VertexTextureCoordinates);
    m_isDirty = true;
    return true;
}

bool Mesh::SetIndices(const FixedMemoryVector<int>& indices)
{
    return SetIndices(indices.Data(), indices.Size());
}

bool Mesh::SetIndices(const int* indexArray, uint32_t arrayLen)
{    
    // No *real* need for conversion here, but for the sake of consistency...
    m_VertexIndices.clear();
    m_VertexIndices.resize(arrayLen);

    memcpy(m_VertexIndices.data(), indexArray, arrayLen * sizeof(int));
    m_Indices.Set(m_VertexIndices);
    m_isDirty = true;
    return true;
}

bool Mesh::SetShader(ShaderPtr spShader)
{
    if (!spShader)
    {
        return false;
    }
    
    m_spShader = spShader;
    m_ModelUniformHandle = spShader->GetUniform("model");
    if (m_ModelUniformHandle == Shader::kInvalidHandleValue)
    {
        LOG(ERROR) << "Could not retrieve expected uniform 'model' from shader '" 
                   << spShader->GetName() << "'";
        return false;
    }

    m_ViewUniformHandle = spShader->GetUniform("view");
    if (m_ViewUniformHandle == Shader::kInvalidHandleValue)
    {
        LOG(ERROR) << "Could not retrieve expected uniform 'view' from shader '" 
                   << spShader->GetName() << "'";
        return false;
    }

    m_ProjUniformHandle = spShader->GetUniform("projection");
    if (m_ProjUniformHandle == Shader::kInvalidHandleValue)
    {
        LOG(ERROR) << "Could not retrieve expected uniform 'projection' from shader '" 
                   << spShader->GetName() << "'";
        return false;
    }

    return true;
}

void Mesh::SetModelMatrix(const glm::mat4& modelMatrix) 
{
    m_ModelMatrix = modelMatrix;
}

void Mesh::SetMeshUniforms(const std::vector<MeshUniformValue>& uniformValues)
{
    m_MeshUniformValues = uniformValues;
}

void Mesh::Draw(const glm::mat4& projMatrix, const glm::mat4& viewMatrix) 
{
    if (!GetVisibility())
    {
        // Nothing to do if we're not visible
        return;
    }
    

    if (!m_spShader || 
         m_ModelUniformHandle == Shader::kInvalidHandleValue ||
         m_ViewUniformHandle  == Shader::kInvalidHandleValue ||
         m_ProjUniformHandle  == Shader::kInvalidHandleValue)
    {
        LOG(WARNING) << "Attempting to render mesh with an improper or invalid shader.";
        return;
    }

    m_spShader->Bind();

    for (MeshUniformValue& uniformValue : m_MeshUniformValues)
    {
        if (uniformValue.Location == Shader::kInvalidHandleValue)
        {
            uniformValue.Location = m_spShader->GetUniform(uniformValue.Name);
        }

        if (uniformValue.Location != Shader::kInvalidHandleValue)
        {
            SetUniformByType(uniformValue.Location, uniformValue.Type, uniformValue.pValue);
        }
        else
        {
            // Log something if we didn't get a valid response in the lookup
            LOG(ERROR) << "Failed to retrieve a location for uniform named '" << uniformValue.Name 
                       << "' in shader '" << m_spShader->GetName() << "'";
        }
    }

    Uniform<glm::mat4>::Set(m_ModelUniformHandle, m_ModelMatrix);
    Uniform<glm::mat4>::Set(m_ViewUniformHandle, viewMatrix);
    Uniform<glm::mat4>::Set(m_ProjUniformHandle, projMatrix);

    glBindVertexArray(m_VAO);

    BindAttributes();

    if (m_isDirty)
    {
        // Update buffer objects
        m_VertexPositionAttribute.Set(m_VertexPositions);
        m_VertexNormalAttribute.Set(m_VertexNormals);
        m_VertexTextureCoordinateAttribute.Set(m_VertexTextureCoordinates);
        m_Indices.Set(m_VertexIndices);
        m_isDirty = false;
    }

    ::Draw(m_Indices, DrawMode::Triangles);
    UnbindAttributes();

    m_spShader->Unbind();

    glBindVertexArray(0);
}

void Mesh::SetVisibility(bool newVisibility)
{
    m_isVisible = newVisibility;
}

bool Mesh::GetVisibility() const
{
    return m_isVisible;
}

