#pragma once

#include <sputter/system/system.h>

namespace sputter { namespace containers {
    template<typename T, int TCapacity>
    class ArrayVector
    {
    public:
        ~ArrayVector()
        {
            Clear();
        }

        void Add(const T& toAdd)
        {
            if (m_size >= TCapacity)
            {
                system::LogAndFail("ArrayVector::Add() - out of bounds");
            }

            m_data[m_size] = toAdd;
            ++m_size;
        }

        void Remove(size_t index)
        {
            if (m_size == 0)
            {
                system::LogAndFail("ArrayVector::Remove() - vector is empty.");
            }

            if (index >= m_size)
            {
                system::LogAndFail("ArrayVector::Remove() - out of bounds.");
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
            if (m_size >= TCapacity)
            {
                system::LogAndFail("ArrayVector::Emplace() - out of bounds");
            }

            new (m_data + m_size) T(std::forward<Args>(args)...);
            ++m_size;
        }

        T* Data() 
        {
            return &m_data[0];
        }

        const T* Data() const
        {
            return &m_data[0];
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

        ArrayVector<T, TCapacity>& operator=(const ArrayVector<T, TCapacity>& other)
        {
            if (TCapacity < other.Size())
            {
                system::LogAndFail("ArrayVector::operator=() - Cannot copy other vector, insufficient capacity.");
            }

            Resize(other.Size());
            for (size_t i = 0; i < Size(); ++i)
            {
                m_data[i] = other[i];
            }

            return *this;
        }

        T& Back()
        {
            if (m_size == 0)
            {
                system::LogAndFail("ArrayVector::Back() - vector is empty.");
            }
            
            return m_data[m_size - 1];
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
            if (newSize > TCapacity)
            {
                system::LogAndFail("ArrayVector::Resize() - resize over capacity.");
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
            return TCapacity;
        }

        // TODO:
        // Iterator support

    private:
        T                            m_data[TCapacity];
        size_t                       m_size = 0;
    };
}}
