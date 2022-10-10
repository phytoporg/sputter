#include "assetstorageprovider.h"

namespace sputter { namespace assets {
    AssetStorageProvider* AssetStorageProvider::s_pAssetStorageProvider = nullptr;

    AssetStorageProvider* AssetStorageProvider::GetAssetStorageProviderAddress()
    {
        return s_pAssetStorageProvider;
    }

    void AssetStorageProvider::SetAssetStorageProviderAddress(AssetStorageProvider* pAssetStorageProvider)
    {
        s_pAssetStorageProvider = pAssetStorageProvider;
    }
}}