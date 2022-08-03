#pragma once

#include "assetdata.h"
#include <sstream>

namespace sputter { namespace assets {
    struct TextData : public AssetData
    {
        virtual AssetDataType GetType() const override
        {
            return AssetDataType::TEXT_ASSET;
        }

        std::stringstream TextStream;
    };
}}