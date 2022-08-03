#include "textfilereader.h"
#include "textdata.h"

#include <fstream>
#include <vector>

using namespace sputter::assets;

bool TextFileReader::TextFileReader::ReadTextFile(const std::string& filePath, TextData* pData) 
{
    std::ifstream in(filePath);
    if (!in.good())
    {
        return false;
    }

    pData->TextStream << in.rdbuf();
    return true;
}
