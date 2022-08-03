#pragma once

#include <string>

namespace sputter { namespace assets {
    struct ImageData;

    class PngReader
    {
    public:
        bool ReadImage(const std::string& imagePath, ImageData* pData);
    };
}}
