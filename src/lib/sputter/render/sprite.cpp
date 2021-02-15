#include "sprite.h"

namespace sputter { namespace render {
    Sprite::Sprite()
        : Sprite(nullptr, 0.0f, 0.0f, 0.0f, 0.0f)
    {}

    Sprite::Sprite(TexturePtr spTexture, float w, float h)
        : Sprite(spTexture, w, h, 0.0f, 0.0f)
    {}

    Sprite::Sprite(TexturePtr spTexture, float w, float h, float x, float y)
        : m_spTexture(spTexture), m_size(w, h), m_position(x, y)
    {}

    void Sprite::SetDimensions(float w, float h)
    {
        m_size = glm::vec2(w, h);
    }

    void Sprite::SetPosition(const math::FPVector2D& position)
    {
        m_position = glm::vec2(
            static_cast<float>(position.GetX()),
            static_cast<float>(position.GetY()));
    }

    void Sprite::SetPosition(float x, float y)
    {
        m_position = glm::vec2(x, y);
    }

    TexturePtr Sprite::GetTexturePtr() const
    {
        return m_spTexture;
    }

    void Sprite::SetTexturePtr(TexturePtr spTexture)
    {
        m_spTexture = spTexture;
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
