#pragma once

#include <sputter/system/system.h>
#include <sputter/memory/fixedmemoryallocator.h>

// 
#include <iostream>
// 

namespace sputter { namespace containers {
    template<typename T>
    class FixedMemoryVector
    {
    public:
        FixedMemoryVector(
            size_t capacity,
            memory::FixedMemoryAllocator& allocator)
            : m_size(0),
              m_capacity(capacity),
              m_data(nullptr),
              m_allocator("FixedMemoryVector")
        {
            if (!allocator.ReserveNext("vector", capacity * sizeof(T), &m_allocator))
            {
                LOG(WARNING) << "FMV::FMV() - out of memory.";
            }

            m_data = reinterpret_cast<T*>(m_allocator.GetBase());
        }

        ~FixedMemoryVector()
        {
            Clear();
        }

        void Add(const T& toAdd)
        {
            if (m_size + 1 > m_capacity)
            {
                LOG(WARNING) << "FMV::Add() - out of bounds";
            }

            m_data[m_size + 1] = toAdd;
            ++m_size;
        }

        template<typename ...Args>
        void Emplace(Args&&... args)
        {
            if (m_size + 1 > m_capacity)
            {
                LOG(WARNING) << "FMV::Add() - out of bounds";
            }

            new (m_data + m_size) T(std::forward<Args>(args)...);
            ++m_size;
        }

        const T& Get(size_t index) const
        {
            return m_data[index];
        }

        T& Get(size_t index)
        {
            return m_data[index];
        }

        const T& operator[](size_t index) const
        {
            return m_data[index];
        }

        T& operator[](size_t index)
        {
            return m_data[index];
        }

        void Clear()
        {
            for (size_t i = 0; i < m_size; ++i)
            {
                T& object = m_data[i];
                object.~T();
            }

            m_size = 0;
        }

        bool Empty() const
        {
            return m_size == 0;
        }

        size_t Size() const
        {
            return m_size;
        }

        size_t Capacity() const
        {
            return m_capacity;
        }

        // TODO:
        // Resize()
        // Remove(index)
        // Iterator support

    private:
        FixedMemoryVector() = delete;

        T*                           m_data;
        size_t                       m_size;
        size_t                       m_capacity;

        memory::FixedMemoryAllocator m_allocator;
    };
}}
