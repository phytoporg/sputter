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
                system::LogAndFail("FMV::Add() - out of bounds");
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
                system::LogAndFail("FMV::Add() - out of bounds");
            }

            m_data[m_size] = toAdd;
            ++m_size;
        }

        void Remove(size_t index)
        {
            if (m_size == 0)
            {
                system::LogAndFail("FMV::Remove() - vector is empty.");
            }

            if (index >= m_size)
            {
                system::LogAndFail("FMV::Remove() - out of bounds.");
            }

            m_data[index].~T();
            for (size_t i = index; i < m_size - 1; ++i)
            {
                m_data[i] = m_data[i + 1];
            }

            m_size--;
        }

        template<typename ...Args>
        void Emplace(Args&&... args)
        {
            if (m_size + 1 > m_capacity)
            {
                system::LogAndFail("FMV::Add() - out of bounds");
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

        void Resize(size_t newSize)
        {
            if (newSize > m_capacity)
            {
                system::LogAndFail("FMV::Resize() - resize over capacity.");
            }

            if (newSize == 0)
            {
                Clear();
                return;
            }

            if (newSize < m_size)
            {
                // Shrink
                for (size_t i = m_size - 1; i > newSize; --i)
                {
                    m_data[i].~T();
                }
            }
            else if (newSize > m_size)
            {
                // In growth case, call default constructors
                for (size_t i = m_size - 1; i < newSize; ++i)
                {
                    m_data[i] = T();
                }
            }

            m_size = newSize;
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
        // Iterator support

    private:
        FixedMemoryVector() = delete;

        T*                           m_data;
        size_t                       m_size;
        size_t                       m_capacity;

        memory::FixedMemoryAllocator m_allocator;
    };
}}
