#pragma once

#include "imagedata.h"
#include <string>

namespace sputter { namespace assets {
    class PngReader
    {
    public:
        bool ReadImage(const std::string& imagePath, ImageData* pData);
    };
}}
