#pragma once

#include <glm/glm.hpp>

namespace sputter { namespace render {
    class SpriteShader
    {
    public:
        SpriteShader();
        ~SpriteShader();

        void SetUniformModelMatrix(const glm::mat4& m);
        void SetUniformProjMatrix(const glm::mat4& m);
        void SetUniformTextureId(int32_t textureId);

        void Use() const;

    private:
        uint32_t m_vertexShaderId;
        uint32_t m_fragmentShaderId;

        uint32_t m_uniformTexId;
        uint32_t m_uniformModelId;
        uint32_t m_uniformProjId;

        uint32_t m_programId;
    };
}}
