#pragma once

#include <cstdint>
#include <vector>

namespace sputter { namespace render {
    class IndexBuffer
    {
    public:
        IndexBuffer();
        IndexBuffer(const IndexBuffer& other);
        IndexBuffer& operator=(const IndexBuffer& other);

        ~IndexBuffer();

        void Set(const uint32_t* inputArray, uint32_t arrayLength);
        void Set(const int* inputArray, uint32_t arrayLength);
        void Set(const std::vector<uint32_t>& data);
        void Set(const std::vector<int>& data);

        uint32_t Count() const;
        uint32_t GetHandle() const;

    private:
        uint32_t m_count = 0;
        uint32_t m_handle = 0;
    };
}}
