#pragma once

namespace sputter { namespace assets {
    enum class AssetDataType
    {
        INVALID = 0,
        IMAGE_ASSET,
        TEXT_ASSET,
        MAX_ASSET_DATA_TYPE
    };

    // An abstract asset data structure
    struct AssetData
    {
        virtual AssetDataType GetType() const = 0;
    };
}}
