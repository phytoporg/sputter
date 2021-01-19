#pragma once

#include "texture.h"
#include <glm/glm.hpp>
#include <vector>

namespace sputter { namespace render {
    class SpriteBatch
    {
    public:
        // TODO: shader param here as well
        SpriteBatch(TexturePtr spTexture, uint32_t maxSpriteCount);

        // Clear sprite data
        void Reset();

        // Fails if the added sprite is not compatible with this batch
        bool AddSprite(const Sprite& sprite);

        // Upload data to GPU
        void Flush();

    private:
        const uint32_t m_maxSpriteCount;

        uint32_t m_vaoId;
        uint32_t m_vboId;
        uint32_t m_idxId;

        TexturePtr m_spTexture;

        struct SpriteVertex
        {
            glm::vec3f Position;
            glm::vec2f TextureCoordinate;
        };

        std::vector<SpriteVertex> m_verticesVector;
        std::vector<uint32_t>     m_indicesVector;
    };
}}
