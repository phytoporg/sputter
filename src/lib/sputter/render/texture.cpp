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
}}
