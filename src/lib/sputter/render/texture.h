#pragma once

#include <memory>
#include <string>

namespace sputter { namespace render { 
    class Texture
    {
    public:
        Texture(const std::string& name, uint32_t id)
            : m_name(name), m_id(id)
        {}

        // Deletes the underlying texture from memory
        ~Texture();

        void Bind();
        void Unbind();

        const std::string& GetName() const;
        const uint32_t GetId() const;

    private:
        Texture() = delete;

        std::string m_name;
        uint32_t    m_id;
    };

    using TexturePtr = std::shared_ptr<Texture>;
}}
