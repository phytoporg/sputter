#include "texture.h"
#include "render.h"

namespace sputter { namespace render { 
    Texture::~Texture()
    {
        Unbind();
        glDeleteTextures(1, &m_id);
    }

    void Texture::Bind()
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_id);
    }

    void Texture::Unbind()
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    const std::string& Texture::GetName() const
    {
        return m_name;
    }

    const uint32_t Texture::GetId() const
    {
        return m_id;
    }
}}
