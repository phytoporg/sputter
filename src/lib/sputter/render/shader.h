#pragma once

#include <vector>
#include <memory>
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
        Shader(
            uint32_t programHandle,
            const std::vector<ShaderAttribute>& attributes,
            const std::vector<ShaderUniform>& uniforms,
            const std::string& name
            );
        ~Shader();

        void Bind();
        void Unbind();

        uint32_t GetAttribute(const std::string& name) const;
        uint32_t GetUniform(const std::string& name) const;
        uint32_t GetHandle() const;

        const std::string& GetName() const;

        static const uint32_t kInvalidHandleValue;

    private:
        // No empty shaders
        Shader() = delete;

        // Don't copy shader objects, as they referencing GPU resources.
        Shader(const Shader& other) = delete;
        Shader& operator=(const Shader& other) = delete;

        // Maps are overkill, just use linear lookups.
        std::vector<ShaderAttribute> m_attributes;
        std::vector<ShaderUniform>   m_uniforms;

        std::string m_name;
        uint32_t m_handle = kInvalidHandleValue;
    };

    using ShaderPtr = std::shared_ptr<Shader>;
}}
