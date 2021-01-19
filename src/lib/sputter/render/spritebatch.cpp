#include "spritebatch.h"
#include <sputter/system/system.h>

namespace sputter { namespace render {
    SpriteBatch::SpriteBatch(TexturePtr spTexture, uint32_t maxSpriteCount)
        : m_vaoId(0), m_vboId(0),
          m_spTexture(spTexture),
          m_maxSpriteCount(maxSpriteCount)
    {
        m_verticesVector.reserve(maxSpriteCount);
        m_indicesVector.reserve(maxSpriteCount);

        glGenBuffers(1, &m_vboId);
        glGenVertexArrays(1, &m_vaoId);
        glGenBuffers(1, &m_idxId);

		glBindVertexArray(m_vaoId);

		glBindBuffer(GL_ARRAY_BUFFER, m_vboId);
		glBufferData(
			GL_ARRAY_BUFFER,
			sizeof(m_verticesVector[0]) * maxSpriteCount * 4,
		    m_verticesVector.data(),
			GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_idxId);
		glBufferData(
			GL_ELEMENT_ARRAY_BUFFER,
			sizeof(m_indicesVector[0]) * 6 * maxSpriteCount,
			m_indicesVector.data(),
			GL_STATIC_DRAW);

        // Position (3 floats)
		glVertexAttribPointer(
            0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);

        // TexCoord (2 floats)
        glVertexAttribPointer(
            1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
            (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
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

        const size_t InitialIndex = m_verticesVector.size();

        const glm::vec3 UlPos = glm::vec3(glm::vec2(SpritePos), 0.0f);
        m_verticesVector.push_back({ UlPos, glm::vec2(0.0f, 0.0f) });

        const glm::vec3 UrPos = UlPos + glm::vec3(SpriteSize.x, 0.0f, 0.0f);
        m_verticesVector.push_back({ UrPos, glm::vec2(1.0f, 0.0f) });

        const glm::vec3 LrPos = UlPos + glm::vec3(SpriteSize.x, SpriteSize.y, 0.0f);
        m_verticesVector.push_back({ LrPos, glm::vec2(1.0f, 1.0f) });

        const glm::vec3 LlPos = UlPos + glm::vec3(0.0f, SpriteSize.y, 0.0f);
        m_verticesVector.push_back({ LlPos, glm::vec2(0.0f, 1.0f) });

        // First triangle (ul, ur, ll)
        m_indicesVector.push_back(InitialIndex);
        m_indicesVector.push_back(InitialIndex + 1);
        m_indicesVector.push_back(InitialIndex + 3);

        // Second triangle (ur, lr, ll)
        m_indicesVector.push_back(InitialIndex + 1);
        m_indicesVector.push_back(InitialIndex + 2);
        m_indicesVector.push_back(InitialIndex + 3);

        return true;
    }

    void SpriteBatch::Flush()
    {
        // TODO: Actually draw stuff!
    }
}}
