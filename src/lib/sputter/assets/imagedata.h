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

        bool     HasAlpha;
        int32_t  Width;
        int32_t  Height;
        int32_t  BitDepth;
        uint8_t* pBytes;
    };
}}
