#pragma once

namespace sputter { namespace assets {
    struct ImageData 
    {
        uint32_t Width;
        uint32_t Height;
        uint32_t BitDepth;
        uint8_t* pBytes;
    };
}}
