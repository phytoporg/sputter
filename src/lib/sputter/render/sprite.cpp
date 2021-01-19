#include "sprite.h"

namespace sputter { namespace render {
    Sprite::Sprite(TexturePtr spTexture, float w, float h)
        : Sprite(spTexture, w, h, 0.0f, 0.0f)
    {}

    Sprite::Sprite(TexturePtr spTexture, float w, float h, float x, float y)
        : m_spTexture(spTexture), m_size(w, h), m_position(x, y)
    {}

    void SetPosition(float x, float y)
    {
        m_position = glm::vec2(x, y);
    }

    TexturePtr Sprite::GetTexturePtr() const
    {
        return m_spTexture;
    }

    glm::vec2 Sprite::GetPosition() const
    {
        return m_position;
    }

    glm::vec2 Sprite::GetSize() const
    {
        return m_size;
    }
}}
