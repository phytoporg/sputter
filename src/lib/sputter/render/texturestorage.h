#pragma once

#include <sputter/assets/imagedata.h>

#include "texture.h"

#include <string>
#include <cstdint>
#include <vector>

namespace sputter { namespace render {
    class TextureStorage
    {
    public:
        bool 
        AddTexture(
            const assets::ImageData& imageData,
            const std::string& textureName
            );

        TexturePtr FindTextureByName(const std::string& textureName) const;

    private:
        // Using a vector for now because I don't anticipate using many 
        // textures in the foreseeable future; linear operations are
        // probably preferable, and we can change that if this things grow in
        // scope and this assumption ever becomes untrue.
        std::vector<TexturePtr> m_storageVector;
    };
}}
