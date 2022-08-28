#include "binaryfilereader.h"
#include "binarydata.h"

#include <fstream>
#include <vector>

using namespace sputter::assets;

bool BinaryFileReader::BinaryFileReader::ReadBinaryFile(const std::string& filePath, BinaryData* pData) 
{
    std::ifstream in(filePath, std::ios_base::binary);
    if (!in.good())
    {
        return false;
    }

    in.seekg(0, std::ios_base::end);
    const size_t FileSize = in.tellg();
    in.seekg(0, std::ios_base::beg);

    pData->spData.reset(new char[FileSize]);
    in.read(pData->spData.get(), FileSize);

    pData->DataLength = FileSize;
    return true;
}
