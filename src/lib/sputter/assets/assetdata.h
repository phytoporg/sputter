#pragma once

namespace sputter { namespace assets {
    enum AssetDataType
    {
        INVALID = 0,
        IMAGE_ASSET,
        // TODO: audio assets, fonts, etc.
        MAX_ASSET_DATA_TYPE
    };

    // An abstract asset data structure
    struct AssetData
    {
        virtual AssetDataType GetType() const = 0;
    };
}}
