#pragma once

#include "texture.h"

#include <string>
#include <cstdint>
#include <vector>

#include <sputter/assets/resourcestorage.h>
#include <sputter/assets/resourcestoragetype.h>

namespace sputter { namespace assets {
    // Forward declarations
    class AssetStorage;
    struct ImageData;
}}

namespace sputter { namespace render {
    class TextureStorage : public sputter::assets::IResourceStorage<Texture>
    {
    public:
        static const assets::ResourceStorageType StorageTypeId =
            assets::ResourceStorageType::TYPE_TEXTURE;

        ~TextureStorage();

        bool 
        AddTextureFromImageAssetName(
            sputter::assets::AssetStorage* pAssetStorage, 
            const std::string& imageAssetName,
            const std::string& textureName
            );

        bool 
        AddTexture(
            const assets::ImageData& imageData,
            const std::string& textureName
            );

        TexturePtr FindTextureByName(const std::string& textureName) const;

    private:
        virtual bool AddResource(Texture* pTexture) override;
        virtual bool ReleaseResource(Texture* pTexture) override;

        // Using a vector for now because I don't anticipate using many 
        // textures in the foreseeable future; linear operations are
        // probably preferable, and we can change that if this things grow in
        // scope and this assumption ever becomes untrue.
        std::vector<TexturePtr> m_storageVector;
    };
}}
