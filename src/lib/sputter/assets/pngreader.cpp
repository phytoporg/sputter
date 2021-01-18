#include "pngreader.h"
#include <sputter/system/system.h>

#include <cstdlib>
#include <cstdio>

#include <png.h>

namespace sputter { namespace assets {
    bool PngReader::ReadImage(const std::string& imagePath, ImageData* pData)
    {
		uint32_t sigRead = 0;
		int colorType;
        int interfaceType;
		FILE *fp;

        pData->pBytes = nullptr;

        if ((fp = fopen(imagePath.c_str(), "rb")) == nullptr)
        {
            LOG(WARNING) << "Failed to open image path " << imagePath;
            return false;
        }

        png_structp pPng = 
            png_create_read_struct(
                PNG_LIBPNG_VER_STRING,
                nullptr,
                nullptr,
                nullptr);
        if (!pPng)
        {
            LOG(WARNING) << "Failed to create png read struct" << imagePath;
            fclose(fp);
            return false;
        }

		png_infop pInfo = png_create_info_struct(pPng);;
        if (!pInfo)
        {
            LOG(WARNING) << "Failed to create png info struct" << imagePath;
            fclose(fp);
            png_destroy_read_struct(&pPng, nullptr, nullptr);
            return false;
        }

        if (setjmp(png_jmpbuf(pPng)))
        {
            LOG(WARNING) << "setjmp failed" << imagePath;
            png_destroy_read_struct(&pPng, nullptr, nullptr);
            fclose(fp);
            return false;
        }

        png_init_io(pPng, fp);
        png_set_sig_bytes(pPng, sigRead);
        png_read_png(
            pPng,
            pInfo,
            PNG_TRANSFORM_STRIP_16 |
            PNG_TRANSFORM_PACKING  |
            PNG_TRANSFORM_EXPAND,
            nullptr);

        png_uint_32 width;
        png_uint_32 height;
        int bitDepth;

        png_get_IHDR(
            pPng, pInfo,
            &width, &height, &bitDepth, &colorType, &interfaceType,
            nullptr, nullptr);
        pData->Width = width;
        pData->Height = height;
        pData->BitDepth = bitDepth;

        uint32_t stride = png_get_rowbytes(pPng, pInfo);
        pData->pBytes = new uint8_t[stride * height];

        png_bytepp rowPointers = png_get_rows(pPng, pInfo);
        for (int i = 0; i < height; ++i)
        {
            // Swap the ordering to appease OpenGL
            memcpy(
                pData->pBytes + (stride * (height - 1 - i)),
                rowPointers[i],
                stride);
        }

        png_destroy_read_struct(&pPng, &pInfo, nullptr);

        fclose(fp);

        return true;
    }
}}
