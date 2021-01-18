#pragma once

#include "assetdata.h"
#include <cstdint>

namespace sputter { namespace assets {
    struct ImageData : public AssetData
    {
        virtual AssetDataType GetType() const 
        {
            return IMAGE_ASSET;
        }

        uint32_t Width;
        uint32_t Height;
        uint32_t BitDepth;
        uint8_t* pBytes;
    };
}}
