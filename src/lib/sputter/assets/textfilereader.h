#pragma once

#include <string>

namespace sputter { namespace assets {
    // Forward declarations
    struct TextData;

    class TextFileReader 
    {
        public:
            bool ReadTextFile(const std::string& filePath, TextData* pData);
    };
}}