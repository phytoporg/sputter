#include "texture.h"

#include <GL/gl.h>

namespace sputter { namespace render { 
    Texture::~Texture()
    {
        Unbind();
        glDeleteTextures(1, &m_id);
    }

    void Texture::Bind()
    {
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
