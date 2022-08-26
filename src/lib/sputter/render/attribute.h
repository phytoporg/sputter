#pragma once

#include "render.h"
#include <vector>

namespace sputter { namespace render { 
    template<typename T>
    class Attribute
    {
    public:
        Attribute();
        Attribute(const Attribute& other);
        Attribute& operator=(const Attribute& other);

        ~Attribute();

        void SetAttributePointer(uint32_t slot, uint32_t stride = 0);
        void Set(T* inputArray, uint32_t arrayLength);
        void Set(std::vector<T>& input);
        void BindTo(uint32_t slot, uint32_t stride = 0);
        void UnbindFrom(uint32_t slot);
        void SetInstanceDivisor(uint32_t divisor);
        uint32_t Count();
        uint32_t GetHandle();

    protected:
        uint32_t m_handle = 0;
        uint32_t m_count = 0;
    };

    template<typename T>
    Attribute<T>::Attribute()
    {
        glGenBuffers(1, &m_handle);
        m_count = 0;
    }

    template<typename T>
    Attribute<T>::Attribute(const Attribute<T>& other)
    {
        // Copy references to underlying resources
        m_handle = other.m_handle;
        m_count = other.m_count;
    }

    template<typename T>
    Attribute<T>& Attribute<T>::operator=(const Attribute<T>& other)
    {
        m_handle = other.m_handle;
        m_count = other.m_count;
        return *this;
    }

    template<typename T>
    Attribute<T>::~Attribute()
    {
        glDeleteBuffers(1, &m_handle);
        m_handle = 0;
        m_count = 0;
    }

    template<typename T>
    uint32_t Attribute<T>::Count()
    {
        return m_count;
    }

    template<typename T>
    uint32_t Attribute<T>::GetHandle()
    {
        return m_handle;
    }

    template<typename T>
    void Attribute<T>::Set(T* inputArray, uint32_t arrayLength)
    {
        m_count = arrayLength;
        size_t size = sizeof(T);
        glBindBuffer(GL_ARRAY_BUFFER, m_handle);
        glBufferData(GL_ARRAY_BUFFER, size * m_count, inputArray, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    template<typename T>
    void Attribute<T>::Set(std::vector<T>& input)
    {
        Set(input.data(), static_cast<uint32_t>(input.size()));
    }

    template<typename T>
    void Attribute<T>::BindTo(uint32_t slot, uint32_t stride)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_handle);
        SetAttributePointer(slot, stride);
        glEnableVertexAttribArray(slot);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    template<typename T>
    void Attribute<T>::UnbindFrom(uint32_t slot)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_handle);
        glDisableVertexAttribArray(slot);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    template<typename T>
    void Attribute<T>::SetInstanceDivisor(uint32_t divisor)
    {
        glVertexAttribDivisor(m_handle, divisor);
    }
} }
