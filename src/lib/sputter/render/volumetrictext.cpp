#include "volumetrictext.h"

#include "render.h"
#include "attribute.h"
#include "indexbuffer.h"
#include "geometry.h"

#include <sputter/system/system.h>

#include <string>
#include <glm/glm.hpp>

using namespace sputter::render;

struct VolumetricTextRenderer::PImpl 
{
    Glyph GlyphLookup[256];
    uint32_t  VAO;
    ShaderPtr spShader;

    // Attributes & EOB
    Attribute<glm::vec3>   VertexPositionAttribute;
    Attribute<glm::vec3>   VertexNormalAttribute;
    Attribute<glm::vec2>   VertexTextureCoordinateAttribute;
    IndexBuffer            Indices;

    // Data
    std::vector<glm::vec3>    VertexPositions;
    std::vector<glm::vec3>    VertexNormals;
    std::vector<glm::vec2>    VertexTextureCoordinates;
    std::vector<int>          VertexIndices;

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

VolumetricTextRenderer::VolumetricTextRenderer(ShaderPtr spShader) 
    : m_spPimpl(std::make_unique<PImpl>())
{
    m_spPimpl->spShader = spShader;

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

    glBindVertexArray(0);
}

void VolumetricTextRenderer::DrawText(uint32_t x, uint32_t y, uint32_t size, const char* pText)
{
    const glm::vec2 rootPos(x, y);
    m_spPimpl->spShader->Bind();

    // TODO: Set uniforms, vbo, etc
    // Reference @ https://learnopengl.com/Advanced-OpenGL/Instancing
}