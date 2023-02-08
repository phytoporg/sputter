#include "texturestorage.h"
#include "render.h"

#include <algorithm>

#include <sputter/assets/assetstorage.h>
#include <sputter/assets/imagedata.h>
#include <sputter/system/system.h>
#include <sputter/log/log.h>

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

using namespace sputter::render;

TextureStorage::~TextureStorage()
{
    while (!m_storageVector.empty())
    {
        ReleaseResource(m_storageVector.front().get());
    }
}

bool 
TextureStorage::AddTextureFromImageAssetName(
    sputter::assets::AssetStorage* pAssetStorage,
    const std::string& imageAssetName,
    const std::string& textureName
    )
{
    auto spImageAsset = pAssetStorage->FindFirstByName(imageAssetName);
    if (!spImageAsset)
    {
        RELEASE_LOGLINE_ERROR(LOG_ASSETS, "Could not find image asset %s for texture storage %s", imageAssetName.c_str(), textureName.c_str());
        return false;
    }

    if (spImageAsset->GetType() != assets::AssetDataType::IMAGE_ASSET)
    {
        RELEASE_LOGLINE_ERROR(LOG_ASSETS, "Asset %s is not an image asset", imageAssetName.c_str());
        return false;
    }

    auto pImageData = dynamic_cast<assets::ImageData*>(spImageAsset.get());
    if (!pImageData->pBytes)
    {
        RELEASE_LOGLINE_ERROR(LOG_ASSETS, "Image asset %s was loaded but has zero bytes!", imageAssetName.c_str());
        return false;
    }

    return AddTexture(*pImageData, textureName);
}

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
        RELEASE_LOGLINE_WARNING(LOG_ASSETS, "Texture named '%s' has dimensions which are not a power of two.", textureName.c_str());
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

    Texture* pTexture = 
        new Texture(
            textureName,
                textureId,
                imageData.Width,
                imageData.Height,
                imageData.HasAlpha ? 4 : 3);
    if (!AddResource(pTexture))
    {
        delete pTexture;
        return false;
    }

    return true;
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

bool TextureStorage::AddResource(Texture* pTexture)
{
    if (!pTexture) { return false; }

    if (!FindTextureByName(pTexture->GetName()))
    {
        m_storageVector.emplace_back(pTexture);
        RELEASE_LOGLINE_INFO(LOG_ASSETS, "Added texture to storage: %s", pTexture->GetName().c_str());
        return true;
    }
    else
    {
        RELEASE_LOGLINE_WARNING(LOG_ASSETS, "Failed to add texture %s to storage: already exists.", pTexture->GetName().c_str());
        return false;
    }
}

bool TextureStorage::ReleaseResource(Texture* pTexture)
{
    auto it = std::find_if(
        std::begin(m_storageVector),
        std::end(m_storageVector),
        [pTexture](const TexturePtr& spTexture) { return spTexture.get() == pTexture; });
    if (it != std::end(m_storageVector))
    {
        m_storageVector.erase(it);
        return true;
    }
    
    RELEASE_LOGLINE_WARNING(LOG_ASSETS, "Failed to remove texture %s from storage: could not locate in storage.", pTexture->GetName().c_str());
    return false;
}
