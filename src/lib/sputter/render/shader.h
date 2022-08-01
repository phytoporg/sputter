#pragma once

#include <vector>
#include <string>

namespace sputter { namespace render {
    struct ShaderAttribute
    {
        ShaderAttribute(const std::string& name, uint32_t location)
            : Name(name), Location(location)
        {}

        std::string Name;
        uint32_t Location;
    };

    struct ShaderUniform
    {
        ShaderUniform(const std::string& name, uint32_t location)
            : Name(name), Location(location)
        {}

        std::string Name;
        uint32_t Location;
    };

    class Shader
    {
    public:
        Shader(); // Do we want to allow empty shader objects?
        Shader(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath);
        ~Shader();

        void Load(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath);

        void Bind();
        void Unbind();

        uint32_t GetAttribute(const std::string& name) const;
        uint32_t GetUniform(const std::string& name) const;
        uint32_t GetHandle() const;

    private:
        // Don't copy shader objects, as they referencing GPU resources.
        Shader(const Shader& other) = delete;
        Shader& operator=(const Shader& other) = delete;

        // TODO: this likely belongs in the asset pipeline, but plopping here for now for expediency.
        std::string ReadFile(const std::string& filePath);

        // Returns shader handle, or 0 if compilation failed.
        uint32_t CompileVertexShader(const std::string& shaderText);
        uint32_t CompileFragmentShader(const std::string& shaderText);

        bool LinkShaders(uint32_t vertexShaderHandle, uint32_t fragmentShaderHandle);

        void PopulateAttributes();
        void PopulateUniforms();

        // Maps are overkill, just use linear lookups.
        std::vector<ShaderAttribute> m_attributes;
        std::vector<ShaderUniform>   m_uniforms;

        static const uint32_t kInvalidHandleValue;
        uint32_t m_handle = kInvalidHandleValue;
    };
}}
