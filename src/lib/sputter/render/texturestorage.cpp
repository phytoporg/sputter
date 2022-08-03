#include "texturestorage.h"
#include "render.h"

#include <sputter/assets/imagedata.h>
#include <sputter/system/system.h>

// Probably belongs in a more visible spot, but this isn't yet needed anywhere else.
namespace {
    static bool IsIntegerPowerOfTwo(int integer)
    {
        // Any power-of-two integer has a single set bit in its binary representation, so ANDing
        // with its decrement will always produce zero. This test only works if the integer 
        // being tested is greater than one to begin with. Since 2^0 == 1, we'll consider that
        // case separately.

        // i.e.
        // Power of two: 1000 & 0111 = 0000
        // Not a power of two: 1010 & 1001 = 1000 <-- More than one bit

        return integer == 1 || (integer > 1 && ((integer - 1) & integer) == 0);
    }
}

namespace sputter { namespace render {
    bool 
    TextureStorage::AddTexture(
        const assets::ImageData& imageData,
        const std::string& textureName
        )
    {
        // Check to see if dimensions are a power of two.
        if (IsIntegerPowerOfTwo(imageData.Width) || IsIntegerPowerOfTwo(imageData.Height))
        {
            // Not catastrophic, but may result in unexpected behaviors. Currently there are no use cases
            // where this is valid, but just keep a warning in case that ever changes.
            LOG(WARNING) << "Texture named '" << textureName << "' has dimensions which are not powers of two.";
        }

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
            Texture* pTexture = 
                new Texture(
                    textureName,
                     textureId,
                     imageData.Width,
                     imageData.Height,
                     imageData.HasAlpha ? 4 : 3);
            if (!pTexture)
            {
                LOG(ERROR) << "Failed to allocate new texture. OOM?";
                return false;
            }
                     
            m_storageVector.emplace_back(pTexture);
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
