#include "texturestorage.h"
#include <sputter/system/system.h>
#include <GL/gl.h>
#include <GL/glu.h>

namespace sputter { namespace render {
    bool 
    TextureStorage::AddTexture(
        const assets::ImageData& imageData,
        const std::string& textureName
        )
    {
        glActiveTexture(GL_TEXTURE0);
        glEnable(GL_TEXTURE_2D);

        GLuint textureId;
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        // Scale linearly when image is larger than texture
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 

        // Scale linearly when image is smaller than texture
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 

        glPixelStorei(GL_PACK_ALIGNMENT, 1); 
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 

        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);

        glTexImage2D(
            GL_TEXTURE_2D, 0, imageData.HasAlpha ? GL_RGBA : GL_RGB,
            imageData.Width, imageData.Height, 
            0, 
            imageData.HasAlpha ? GL_RGBA : GL_RGB,
            GL_UNSIGNED_BYTE,
            imageData.pBytes);

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
        for (const TexturePtr& spTexture : m_storageVector)
        {
            if (spTexture->GetName() == textureName)
            {
                return spTexture;
            }
        }

        return nullptr;
    }
}}
