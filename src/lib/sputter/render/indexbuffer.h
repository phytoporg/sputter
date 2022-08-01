#pragma once

#include <cstdint>
#include <vector>

namespace sputter { namespace render {
    class IndexBuffer
    {
    public:
        IndexBuffer();
        ~IndexBuffer();

        void Set(const uint32_t* inputArray, uint32_t arrayLength);
        void Set(const std::vector<uint32_t>& data);

        uint32_t Count() const;
        uint32_t GetHandle() const;

    private:
        IndexBuffer(const IndexBuffer& other) = delete;
        IndexBuffer& operator=(const IndexBuffer& other) = delete;

        uint32_t m_count = 0;
        uint32_t m_handle = 0;
    };
}}
