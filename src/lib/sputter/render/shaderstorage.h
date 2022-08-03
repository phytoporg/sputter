#pragma once

#include "shader.h"
#include <string>
#include <vector>
#include <cstdint>

// Forward declarations
namespace sputter { namespace assets {
    struct TextData;
}}

namespace sputter { namespace render {
    class ShaderStorage
    {
    public:
        bool
        AddShader(
            const assets::TextData& vertexText,
            const assets::TextData& fragmentText,
            const std::string& shaderName
            );

        ShaderPtr FindShaderByName(const std::string& shaderName) const;

    private:
        uint32_t CompileVertexShader(const std::string& source);
        uint32_t CompileFragmentShader(const std::string& source);

        // TODO:
        // LinkShaders
        bool 
        LinkShaders(
            uint32_t programHandle,
            uint32_t vertexShaderHandle,
            uint32_t fragmentShaderHandle
            );
        std::vector<ShaderAttribute> PopulateAttributes(uint32_t programHandle);
        std::vector<ShaderUniform> PopulateUniforms(uint32_t programHandle);

        std::vector<ShaderPtr> m_storageVector;
    };
}}