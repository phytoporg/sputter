#pragma once

#include <memory>
#include <string>

namespace sputter { namespace render { 
    class Texture
    {
    public:
        Texture(
            const std::string& name,
             uint32_t handle,
             uint32_t width,
             uint32_t height,
             uint32_t channels)
            : m_name(name),
              m_handle(handle),
              m_width(width),
              m_height(height),
              m_channels(channels)
        {}

        // Deletes the underlying texture from memory
        ~Texture();

        void Bind();
        void Unbind();

        const std::string& GetName() const;
        const uint32_t GetHandle() const;

    private:
        Texture() = delete;

        std::string m_name;
        uint32_t    m_handle;

        uint32_t    m_width;
        uint32_t    m_height;
        uint32_t    m_channels;
    };

    using TexturePtr = std::shared_ptr<Texture>;
}}
