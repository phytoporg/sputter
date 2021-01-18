#include "texturestorage.h"
#include <sputter/system/system.h>
#include <GL/gl.h>

namespace sputter { namespace render {
    bool 
    TextureStorage::AddTexture(
        const assets::ImageData& imageData,
        const std::string& textureName
        )
    {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        GLuint textureId;
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        // Scale linearly when image is larger than texture
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 

        // Scale linearly when image is smaller than texture
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 

        glTexImage2D(
            GL_TEXTURE_2D, 0, imageData.HasAlpha ? 4 : 3,
            imageData.Width, imageData.Height, 
            0, 
            imageData.HasAlpha ? GL_RGBA : GL_RGB,
            GL_UNSIGNED_BYTE,
            imageData.pBytes);

        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glEnable(GL_TEXTURE_2D);
        glShadeModel(GL_FLAT);

        if (!FindTextureByName(textureName))
        {
            m_storageVector.emplace_back(new Texture(textureName, textureId));
            LOG(INFO) << "Added texture to storage: " << textureName;
            return true;
        }
        else
        {
            LOG(WARNING) << "Failed to add texture " << textureName 
                         << ": already exists.";
            return false;
        }
    }

    TexturePtr 
    TextureStorage::FindTextureByName(const std::string& textureName) const
    {
        // TODO
        return std::shared_ptr<Texture>(nullptr);
    }
}}
