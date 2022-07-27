#pragma once

#include "render.h"
#include <vector>

namespace sputter { namespace render { 
    template<typename T>
    class Attribute
    {
    public:
        Attribute();
        ~Attribute();

        void SetAttributePointer(uint32_t slot);
        void Set(T* inputArray, uint32_t arrayLength);
        void Set(std::vector<T>& input);
        void BindTo(uint32_t slot);
        void UnbindFrom(uint32_t slot);
        uint32_t Count();
        uint32_t GetHandle();

    protected:
        uint32_t m_handle;
        uint32_t m_count;

    private:
        Attribute(const Attribute& other);
        Attribute& operator=(const Attribute& other);
    };

    template<typename T>
    Attribute<T>::Attribute()
    {
        glGenBuffers(1, &m_handle);
        m_count = 0;
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
        glBufferData(GL_ARRAY_BUFFER, size * m_count, inputArray, GL_STREAM_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    template<typename T>
    void Attribute<T>::Set(std::vector<T>& input)
    {
        Set(input.data(), static_cast<uint32_t>(input.size()));
    }

    template<typename T>
    void Attribute<T>::BindTo(uint32_t slot)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_handle);
        glEnableVertexAttribArray(slot);
        SetAttributePointer(slot);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    template<typename T>
    void Attribute<T>::UnbindFrom(uint32_t slot)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_handle);
        glDisableVertexAttribArray(slot);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
} }
