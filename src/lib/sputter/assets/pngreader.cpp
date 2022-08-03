#include "pngreader.h"
#include "imagedata.h"
#include <sputter/system/system.h>

#include <cstdlib>
#include <cstdio>

#include <png.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace sputter { namespace assets {
    bool PngReader::ReadImage(const std::string& imagePath, ImageData* pData)
    {
        pData->pBytes = 
            stbi_load(
                imagePath.c_str(),
                &pData->Width,
                &pData->Height,
                &pData->BitDepth,
                STBI_rgb_alpha);
        if (!pData->pBytes)
        {
            LOG(WARNING) << "Failed to load image: " << imagePath;
            return false;
        }

        pData->HasAlpha = pData->BitDepth == 4;

        return true;
    }
}}
