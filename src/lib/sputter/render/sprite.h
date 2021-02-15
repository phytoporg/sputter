#pragma once

#include <glm/glm.hpp>
#include <sputter/math/fpvector2d.h>
#include "texture.h"

namespace sputter { namespace render {
    class Sprite
    {
    public:
        // TODO (philjo 1/18/2021): should use fixed point floats here and 
        // convert to glm at rendering stages.
        Sprite();
        Sprite(TexturePtr spTexture, float w, float h);
        Sprite(TexturePtr spTexture, float w, float h, float x, float y);

        void SetDimensions(float w, float h);

        void SetPosition(const math::FPVector2D& position);
        void SetPosition(float x, float y);

        void SetTexturePtr(TexturePtr spTexture);
        TexturePtr GetTexturePtr() const;

        glm::vec2 GetPosition() const;
        glm::vec2 GetSize() const;

        // Eventually:
        // rotation, scale

    private:
        glm::vec2 m_position;
        glm::vec2 m_size;

        TexturePtr m_spTexture;
    };
}}
