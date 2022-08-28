#pragma once

#include "assetdata.h"
#include <memory>

namespace sputter { namespace assets {
    struct BinaryData : public AssetData
    {
        virtual AssetDataType GetType() const override
        {
            return AssetDataType::BINARY_ASSET;
        }

        std::unique_ptr<char[]> spData;
        size_t                  DataLength;
    };
}}