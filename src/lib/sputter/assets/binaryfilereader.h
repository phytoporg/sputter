#pragma once

#include <string>

namespace sputter { namespace assets {
    // Forward declarations
    struct BinaryData;

    class BinaryFileReader 
    {
    public:
        bool ReadBinaryFile(const std::string& filePath, BinaryData* pData);
    };
}}