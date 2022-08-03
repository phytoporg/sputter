#include "spritebatch.h"
#include "render.h"
#include "shader.h"
#include "uniform.h"

#include <sputter/system/system.h>

namespace sputter { namespace render {
    SpriteBatch::SpriteBatch(uint32_t maxSpriteCount)
        : SpriteBatch(nullptr, maxSpriteCount)
    {}

    SpriteBatch::SpriteBatch(TexturePtr spTexture, uint32_t maxSpriteCount)
        : m_vaoId(0), m_vboId(0),
          m_spTexture(spTexture),
          m_maxSpriteCount(static_cast<size_t>(maxSpriteCount)) // TODO: Decide on a type!
    {
        // 4 verts per quad
        m_verticesVector.reserve(maxSpriteCount * 4);

        // 6 indices per quad to identify two triangles
        m_indicesVector.reserve(maxSpriteCount * 6);

        glGenVertexArrays(1, &m_vaoId);
        glGenBuffers(1, &m_vboId);
        glGenBuffers(1, &m_idxId);

        glBindVertexArray(m_vaoId);

        glBindBuffer(GL_ARRAY_BUFFER, m_vboId);
        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(m_verticesVector[0]) * maxSpriteCount * 4,
            m_verticesVector.data(),
            GL_DYNAMIC_DRAW);

        // Position (3 floats)
        glVertexAttribPointer(
            0, 3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(SpriteVertex),
            (GLvoid*)0);
        glEnableVertexAttribArray(0);

        // TexCoord (2 floats)
        glVertexAttribPointer(
            1, 2,
            GL_FLOAT,
            GL_FALSE,
            sizeof(SpriteVertex),
            reinterpret_cast<void*>(offsetof(SpriteVertex, TextureCoordinate)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_idxId);
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            sizeof(m_indicesVector[0]) * 6 * maxSpriteCount,
            m_indicesVector.data(),
            GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    SpriteBatch::~SpriteBatch()
    {
        glDeleteBuffers(1, &m_vaoId);
        glDeleteBuffers(1, &m_vboId);
        glDeleteBuffers(1, &m_idxId);
    }

    void SpriteBatch::Reset()
    {
        m_verticesVector.clear();
        m_indicesVector.clear();
    }

    bool SpriteBatch::AddSprite(const Sprite& sprite)
    {
        if (sprite.GetTexturePtr()->GetName() != m_spTexture->GetName())
        {
            LOG(WARNING) << "Mismatched sprite texture: " 
                         << sprite.GetTexturePtr()->GetName()
                         << ", current batch is using "
                         << m_spTexture->GetName();
            return false;
        }

        const size_t CurrentSpriteCount = m_verticesVector.size() / 4;
        if (CurrentSpriteCount > m_maxSpriteCount)
        {
            LOG(WARNING) << "Sprite batch size (" 
                         << CurrentSpriteCount
                         << ") has grown beyond capacity ("
                         << m_maxSpriteCount
                         << ")";
            return false;
        }

        const glm::vec2 SpritePos = sprite.GetPosition();
        const glm::vec2 SpriteSize = sprite.GetSize();

        const uint32_t InitialIndex = static_cast<uint32_t>(m_verticesVector.size());

        const glm::vec3 UlPos = glm::vec3(glm::vec2(SpritePos), 1.0f);
        m_verticesVector.push_back({ UlPos, glm::vec2(0.0f, 0.0f) });

        const glm::vec3 UrPos = UlPos + glm::vec3(SpriteSize.x, 0.0f, 0.0f);
        m_verticesVector.push_back({ UrPos, glm::vec2(1.0f, 0.0f) });

        const glm::vec3 LrPos = UlPos + glm::vec3(SpriteSize.x, SpriteSize.y, 0.0f);
        m_verticesVector.push_back({ LrPos, glm::vec2(1.0f, 1.0f) });

        const glm::vec3 LlPos = UlPos + glm::vec3(0.0f, SpriteSize.y, 0.0f);
        m_verticesVector.push_back({ LlPos, glm::vec2(0.0f, 1.0f) });

        // First triangle (ul, ur, lr)
        m_indicesVector.push_back(InitialIndex + 1);
        m_indicesVector.push_back(InitialIndex   );
        m_indicesVector.push_back(InitialIndex + 3);

        // Second triangle (ul, lr, ll)
        m_indicesVector.push_back(InitialIndex + 1);
        m_indicesVector.push_back(InitialIndex + 3);
        m_indicesVector.push_back(InitialIndex + 2);

        return true;
    }

    void SpriteBatch::Draw(Shader* pShader)
    {
        glActiveTexture(GL_TEXTURE0);
        glEnable(GL_TEXTURE_2D);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_idxId); 
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            sizeof(m_indicesVector[0]) * m_indicesVector.size(),
            m_indicesVector.data(),
            GL_DYNAMIC_DRAW);

        glBindVertexArray(m_vaoId);
        glBindBuffer(GL_ARRAY_BUFFER, m_vboId);
        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(m_verticesVector[0]) * m_verticesVector.size(),
            m_verticesVector.data(),
            GL_DYNAMIC_DRAW);

        const uint32_t UniformTextureHandle = pShader->GetUniform("image");
        Uniform<int>::Set(UniformTextureHandle, 0);
        m_spTexture->Bind();

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_idxId); 
        glDrawElements(
            GL_TRIANGLES,
            static_cast<GLsizei>(m_indicesVector.size()),
            GL_UNSIGNED_INT,
            nullptr);

        m_spTexture->Unbind();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);   
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void SpriteBatch::SetTexturePtr(TexturePtr spTexture)
    {
        m_spTexture = spTexture;
    }

    TexturePtr SpriteBatch::GetTexturePtr()
    {
        return m_spTexture;
    }
}}
