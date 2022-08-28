#pragma once

#include "font.h"

#include <string>

#include <sputter/assets/resourcestorage.h>
#include <sputter/assets/resourcestoragetype.h>

namespace sputter {
    namespace assets { 
        struct BinaryData; 
        class AssetStorage; 
    }
}

namespace sputter { namespace render {
    class FontStorage : public assets::IResourceStorage<Font>
    {
    public:
        static const assets::ResourceStorageType StorageTypeId = 
            assets::ResourceStorageType::TYPE_FONT;

        bool
        AddFontFromAssetName(
            assets::AssetStorage* pAssetStorage,
            const std::string& fontAssetName
        );

        bool
        AddFont(
            const assets::BinaryData& binaryData,
            const std::string& fontName
        );

        FontPtr FindFontByName(const std::string& fontName) const;
    private:
        virtual bool AddResource(Font* pTexture) override;
        virtual bool ReleaseResource(Font* pTexture) override;

        std::vector<FontPtr> m_storageVector;
    };
}}