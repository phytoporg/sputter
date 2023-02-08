#include "volumetrictext.h"

#include "render.h"
#include "shader.h"
#include "shaderstorage.h"
#include "attribute.h"
#include "uniform.h"
#include "indexbuffer.h"
#include "geometry.h"
#include "draw.h"
#include "font.h"
#include "fontstorage.h"

#include <sputter/system/system.h>
#include <sputter/log/log.h>

#include <string>
#include <vector>
#include <glm/glm.hpp>

static const float kDefaultDepth = 0.f; 
static const int kMaxInstances = 2000;
static const char* kVertexShaderName = "volume_text_vert";
static const char* kFragmentShaderName = "volume_text_frag";
static const char* kShaderName = "volumetext_shader";
static const char* kFontName = "fixedsys_font";

using namespace sputter::render;

struct VolumetricTextRenderer::PImpl 
{
    uint32_t  VAO;
    ShaderPtr spShader;
    FontPtr   spFont;

    uint32_t                  OffsetPositionUniformHandle   = Shader::kInvalidHandleValue;
    uint32_t                  ViewMatrixUniformHandle       = Shader::kInvalidHandleValue;
    uint32_t                  ProjectionMatrixUniformHandle = Shader::kInvalidHandleValue;
    uint32_t                  SizeUniformHandle             = Shader::kInvalidHandleValue;

    glm::mat4                 ProjectionMatrix;
    glm::mat4                 ViewMatrix;

    float                     Depth = kDefaultDepth;

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

VolumetricTextRenderer::VolumetricTextRenderer(
    assets::AssetStorage* pAssetStorage,
    ShaderStorage* pShaderStorage,
    FontStorage* pFontStorage
    ) 
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

    // TODO: Actually use this font
    m_spPimpl->spFont = pFontStorage->FindFontByName(kFontName);
    if (!m_spPimpl->spFont)
    {
        if (!pFontStorage->AddFontFromAssetName(pAssetStorage, kFontName))
        {
            system::LogAndFail("Could not add volumetric text font to storage!");
        }
        m_spPimpl->spFont = pFontStorage->FindFontByName(kFontName);
    }

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

VolumetricTextRenderer::~VolumetricTextRenderer() {}

void VolumetricTextRenderer::SetMatrices(const glm::mat4& projMatrix, const glm::mat4& viewMatrix)
{
    m_spPimpl->ProjectionMatrix = projMatrix;
    m_spPimpl->ViewMatrix = viewMatrix;
}

void VolumetricTextRenderer::SetDepth(float depth)
{
    m_spPimpl->Depth = depth;
}

float VolumetricTextRenderer::GetDepth() const
{
    return m_spPimpl->Depth;
}

void VolumetricTextRenderer::DrawText(int32_t x, int32_t y, uint32_t size, const char* pText)
{
    // Adjust the depth so that the *front* face of the voxels are at the specified depth
    // since these coordinates represent the centered origin of the unit cube.
    const glm::vec3 rootPos(x, y, m_spPimpl->Depth + (size * 0.5f));
    m_spPimpl->spShader->Bind();

    if (m_spPimpl->OffsetPositionUniformHandle == Shader::kInvalidHandleValue)
    {
        m_spPimpl->OffsetPositionUniformHandle = m_spPimpl->spShader->GetUniform("rootPos");
        if (m_spPimpl->OffsetPositionUniformHandle == Shader::kInvalidHandleValue)
        {
            system::LogAndFail("Failed to retrieve uniform handle for 'rootPos'");
        }
    }
    Uniform<glm::vec3>::Set(m_spPimpl->OffsetPositionUniformHandle, rootPos);

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
        Glyph characterGlyph;
        if (!m_spPimpl->spFont->GetGlyph(*pCurrentCharacter, &characterGlyph))
        {
            system::LogAndFail("Could not get character glyph.");
        }

        if (currentGlyphOffsetX > 0)
        {
            currentGlyphOffsetX += characterGlyph.Metrics.BearingX;
        }

        for (int i = 0; i < characterGlyph.Metrics.Width; ++i)
        {
            for (int j = 0; j < characterGlyph.Metrics.Height; ++j)
            {
                if (characterGlyph.pGlyphPixels[j * characterGlyph.Metrics.Width + i] == 0)
                {
                    continue;
                }

                const float xOffset = currentGlyphOffsetX + i;
                const float yOffset = j;

                m_spPimpl->InstanceOffsets[voxelCount] = glm::vec2(xOffset, yOffset);
                voxelCount++;

                if (voxelCount > kMaxInstances)
                {
                    system::LogAndFail("Too many voxels for text!");
                }
            }
        }

        currentGlyphOffsetX += characterGlyph.Metrics.Width;
        pCurrentCharacter++;
    }

    glBindVertexArray(m_spPimpl->VAO);
    m_spPimpl->BindAttributes();

    m_spPimpl->InstanceOffsetAttribute.Set(m_spPimpl->InstanceOffsets.data(), voxelCount);

    DrawInstanced(m_spPimpl->Indices, DrawMode::Triangles, voxelCount);

    m_spPimpl->UnbindAttributes();
    m_spPimpl->spShader->Unbind();
}

void VolumetricTextRenderer::DrawTextCentered(int32_t xLeft, int32_t xRight, int32_t yMid, uint32_t size, const char* pText)
{
    if (xRight <= xLeft)
    {
        RELEASE_LOG_ERROR_(LOG_FONT, "DrawTextCenteredHorizontal: xRight must be larger than xLeft");
        return;
    }

    uint32_t maxHeight = 0;
    uint32_t currentGlyphOffsetX = 0;
    const char* pCurrentCharacter = pText;
    while (*pCurrentCharacter)
    {
        GlyphMetrics glyphMetrics;
        if (!m_spPimpl->spFont->GetGlyphMetrics(*pCurrentCharacter, &glyphMetrics))
        {
            system::LogAndFail("Could not get character glyph metrics.");
        }

        if (currentGlyphOffsetX > 0)
        {
            currentGlyphOffsetX += glyphMetrics.BearingX;
        }

        maxHeight = maxHeight > glyphMetrics.Height ? maxHeight : glyphMetrics.Height;
        currentGlyphOffsetX += glyphMetrics.Width;
        pCurrentCharacter++;
    }

    const int32_t xSpan = xRight - xLeft;
    if (xSpan < currentGlyphOffsetX * size)
    {
        RELEASE_LOG_ERROR_(LOG_FONT, "DrawTextCenteredHorizontal: Text is too large to fit between xLeft and xRight");
        return;
    }

    const int32_t startX = xLeft + (xSpan - currentGlyphOffsetX * size) / 2;
    const int32_t startY = yMid - (maxHeight / 2);
    DrawText(startX, startY, size, pText);
}