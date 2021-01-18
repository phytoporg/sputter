#pragma once

#include "assetdata.h"
#include <string>
#include <memory>
#include <map>

namespace sputter { namespace assets {
    class AssetStorage
    {
    public:
        // The asset root path contains all assets for a game, organized by
        // directory hierarchy.
        //
        // TODO(philjo 1/18/2021): For shipping titles it probably makes more
        // sense to load a pre-serialized asset blob. We'll get there later, 
        // though.
        AssetStorage(const std::string& rootPath);

        // Get the first matching asset by name. Returns nullptr if there's no
        // matching asset.
        //
        // This is an O(n) search over assets, terminating the search as soon
        // as a match is found.
        std::shared_ptr<AssetData> 
        FindFirstByName(const std::string& assetName) const;

    private:
        struct AssetStorageEntry
        {
            std::string                Name;
            std::shared_ptr<AssetData> spAssetData;
        };

        // Key is the full relative path
        std::map<std::string, AssetStorageEntry> m_assetMap;
    };
}}
