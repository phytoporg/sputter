#include "volumetrictext.h"

#include "render.h"
#include "shader.h"
#include "shaderstorage.h"
#include "attribute.h"
#include "uniform.h"
#include "indexbuffer.h"
#include "geometry.h"
#include "draw.h"

#include <sputter/system/system.h>

#include <string>
#include <glm/glm.hpp>

static const int kMaxInstances = 1000;
static const char* kVertexShaderName = "volume_text_vert";
static const char* kFragmentShaderName = "volume_text_frag";
static const char* kShaderName = "volumetext_shader";

using namespace sputter::render;

struct VolumetricTextRenderer::PImpl 
{
    Glyph GlyphLookup[256];
    uint32_t  VAO;
    ShaderPtr spShader;

    uint32_t                  OffsetPositionUniformHandle   = Shader::kInvalidHandleValue;
    uint32_t                  ViewMatrixUniformHandle       = Shader::kInvalidHandleValue;
    uint32_t                  ProjectionMatrixUniformHandle = Shader::kInvalidHandleValue;
    uint32_t                  SizeUniformHandle             = Shader::kInvalidHandleValue;

    glm::mat4                 ProjectionMatrix;
    glm::mat4                 ViewMatrix;

    // Attributes & EOB
    Attribute<glm::vec3>      VertexPositionAttribute;
    Attribute<glm::vec3>      VertexNormalAttribute;
    Attribute<glm::vec2>      VertexTextureCoordinateAttribute;
    Attribute<glm::vec2>      InstanceOffsetAttribute;
    IndexBuffer               Indices;

    // Data
    std::vector<glm::vec3>    VertexPositions;
    std::vector<glm::vec3>    VertexNormals;
    std::vector<glm::vec2>    VertexTextureCoordinates;
    std::vector<int>          VertexIndices;
    std::vector<glm::vec2>    InstanceOffsets;

    void BindAttributes()
    {
        VertexPositionAttribute.BindTo(0);
        VertexNormalAttribute.BindTo(1);
        VertexTextureCoordinateAttribute.BindTo(2);
        InstanceOffsetAttribute.BindTo(3, sizeof(glm::vec2));
    }

    void UnbindAttributes()
    {
        VertexPositionAttribute.UnbindFrom(0);
        VertexNormalAttribute.UnbindFrom(1);
        VertexTextureCoordinateAttribute.UnbindFrom(2);
        InstanceOffsetAttribute.UnbindFrom(3);
    }
};

VolumetricTextRenderer::VolumetricTextRenderer(assets::AssetStorage* pAssetStorage, ShaderStorage* pShaderStorage) 
    : m_spPimpl(std::make_unique<PImpl>())
{
    m_spPimpl->spShader = pShaderStorage->FindShaderByName(kShaderName);
    if (!m_spPimpl->spShader)
    {
        if (!pShaderStorage->AddShaderFromShaderAssetNames(
            pAssetStorage,
            kVertexShaderName,
            kFragmentShaderName,
            kShaderName
            ))
        {
            system::LogAndFail("Could not add volumetric text shader to storage!");
        }
        m_spPimpl->spShader = pShaderStorage->FindShaderByName(kShaderName);
    }

    // Just a single "zero" glyph for now
    static bool ZeroGlyphBits[] = 
        { 0, 1, 1, 1, 1, 1, 1, 0,
          1, 1, 1, 1, 1, 1, 1, 1,
          1, 1, 0, 0, 0, 0, 1, 1,
          1, 1, 0, 0, 0, 0, 1, 1,
          1, 1, 0, 0, 0, 0, 1, 1,
          1, 1, 0, 0, 0, 0, 1, 1,
          1, 1, 1, 1, 1, 1, 1, 1,
          0, 1, 1, 1, 1, 1, 1, 0, 
          };

    m_spPimpl->GlyphLookup[static_cast<int>('0')] = Glyph{
        8, 8, ZeroGlyphBits
    };

    glGenVertexArrays(1, &m_spPimpl->VAO);
    glBindVertexArray(m_spPimpl->VAO);

    const uint32_t kNumVertices = 8;
    const uint32_t kNumIndices = 36;
    m_spPimpl->VertexPositions.resize(kNumVertices);
    m_spPimpl->VertexNormals.resize(kNumVertices);
    m_spPimpl->VertexTextureCoordinates.resize(kNumVertices);
    m_spPimpl->VertexIndices.resize(kNumIndices);

    if (!geometry::MakeUnitCube(
        m_spPimpl->VertexPositions.data(), kNumVertices,
        m_spPimpl->VertexNormals.data(), kNumVertices,
        m_spPimpl->VertexTextureCoordinates.data(), kNumVertices,
        m_spPimpl->VertexIndices.data(), kNumIndices
        ))
    {
        sputter::system::LogAndFail("Could not generate unit cube.");
    }

    m_spPimpl->VertexPositionAttribute.Set(m_spPimpl->VertexPositions.data(), kNumVertices);
    m_spPimpl->VertexPositionAttribute.BindTo(0);

    m_spPimpl->VertexNormalAttribute.Set(m_spPimpl->VertexNormals.data(), kNumVertices);
    m_spPimpl->VertexNormalAttribute.BindTo(1);

    m_spPimpl->VertexTextureCoordinateAttribute.Set(m_spPimpl->VertexTextureCoordinates.data(), kNumVertices);
    m_spPimpl->VertexTextureCoordinateAttribute.BindTo(2);

    m_spPimpl->Indices.Set(m_spPimpl->VertexIndices.data(), kNumIndices);

    // Make room for our instance offsets, though we don't yet have useful data
    m_spPimpl->InstanceOffsets.reserve(kMaxInstances);
    m_spPimpl->InstanceOffsetAttribute.Set(m_spPimpl->InstanceOffsets.data(), kMaxInstances);
    m_spPimpl->InstanceOffsetAttribute.BindTo(3, sizeof(float) * 2);
    m_spPimpl->InstanceOffsetAttribute.SetInstanceDivisor(3, 1);

    glBindVertexArray(0);
}

void VolumetricTextRenderer::SetMatrices(const glm::mat4& projMatrix, const glm::mat4& viewMatrix)
{
    m_spPimpl->ProjectionMatrix = projMatrix;
    m_spPimpl->ViewMatrix = viewMatrix;
}

void VolumetricTextRenderer::DrawText(uint32_t x, uint32_t y, uint32_t size, const char* pText)
{
    const glm::vec2 rootPos(x, y);
    m_spPimpl->spShader->Bind();

    if (m_spPimpl->OffsetPositionUniformHandle == Shader::kInvalidHandleValue)
    {
        m_spPimpl->OffsetPositionUniformHandle = m_spPimpl->spShader->GetUniform("rootPos");
        if (m_spPimpl->OffsetPositionUniformHandle == Shader::kInvalidHandleValue)
        {
            system::LogAndFail("Failed to retrieve uniform handle for 'rootPos'");
        }
    }
    Uniform<glm::vec2>::Set(m_spPimpl->OffsetPositionUniformHandle, glm::vec2(x, y));

    if (m_spPimpl->ViewMatrixUniformHandle == Shader::kInvalidHandleValue)
    {
        m_spPimpl->ViewMatrixUniformHandle = m_spPimpl->spShader->GetUniform("view");
        if (m_spPimpl->ViewMatrixUniformHandle == Shader::kInvalidHandleValue)
        {
            system::LogAndFail("Failed to retrieve uniform handle for 'view'");
        }
    }
    Uniform<glm::mat4>::Set(m_spPimpl->ViewMatrixUniformHandle, m_spPimpl->ViewMatrix);

    if (m_spPimpl->ProjectionMatrixUniformHandle == Shader::kInvalidHandleValue)
    {
        m_spPimpl->ProjectionMatrixUniformHandle = m_spPimpl->spShader->GetUniform("projection");
        if (m_spPimpl->ProjectionMatrixUniformHandle == Shader::kInvalidHandleValue)
        {
            system::LogAndFail("Failed to retrieve uniform handle for 'projection'");
        }
    }
    Uniform<glm::mat4>::Set(m_spPimpl->ProjectionMatrixUniformHandle, m_spPimpl->ProjectionMatrix);

    if (m_spPimpl->SizeUniformHandle == Shader::kInvalidHandleValue)
    {
        m_spPimpl->SizeUniformHandle = m_spPimpl->spShader->GetUniform("size");
        if (m_spPimpl->SizeUniformHandle == Shader::kInvalidHandleValue)
        {
            system::LogAndFail("Failed to retrieve uniform handle for 'size'");
        }
    }
    Uniform<float>::Set(m_spPimpl->SizeUniformHandle, size);
    
    // Set up the text !
    uint32_t voxelCount = 0;
    int currentGlyphOffsetX = 0;
    const char* pCurrentCharacter = pText;
    while (*pCurrentCharacter)
    {
        // TODO: safer lookup
        const Glyph* pGlyph = &m_spPimpl->GlyphLookup[pCurrentCharacter[0]];
        for (int x = 0; x < pGlyph->Width; ++x)
        {
            for (int y = 0; y < pGlyph->Height; ++y)
            {
                if (pGlyph->pBitMatrix[y * pGlyph->Width + x] == 0)
                {
                    continue;
                }

                const float xOffset = currentGlyphOffsetX + x;
                const float yOffset = -y;

                m_spPimpl->InstanceOffsets[voxelCount] = glm::vec2(x, y);
                voxelCount++;

                if (voxelCount > kMaxInstances)
                {
                    system::LogAndFail("Too many voxels for text!");
                }
                
            }
        }

        // 1 separates the characters. Should be configurable?
        currentGlyphOffsetX += pGlyph->Width + 1;
        pCurrentCharacter++;
    }

    glBindVertexArray(m_spPimpl->VAO);
    m_spPimpl->BindAttributes();

    m_spPimpl->InstanceOffsetAttribute.Set(m_spPimpl->InstanceOffsets.data(), voxelCount);

    DrawInstanced(m_spPimpl->Indices, DrawMode::Triangles, voxelCount);

    m_spPimpl->UnbindAttributes();
    m_spPimpl->spShader->Unbind();
}