#pragma once

#include <cstdint>

#include <sputter/core/check.h>

namespace sputter { namespace containers {
    template<typename T, int TCapacity>
    class CircularBuffer
    {
    public:
        static_assert(_builtin_popcount(TCapacity) == 1, "Circular buffer capacity must be a power of 2");

        CircularBuffer()
        {
            memset(m_data, 0, sizeof(m_data));
        }

        ~CircularBuffer()
        {
            Clear();
        }

        void Add(const T& toAdd)
        {
            RELEASE_CHECK(!IsFull(), "Attempting to add to full circular buffer");
            m_data[TailIndex()] = toAdd;

            ++m_tail;
        }

        void RemoveFromHead()
        {
            RELEASE_CHECK(IsEmpty(), "Attempting to remove entry from empty circular buffer");

            m_data[HeadIndex()].~T();
            memset(&m_data[HeadIndex()], 0, sizeof(T));

            ++m_head;
        }

        template<typename ...Args>
        void Emplace(Args&&... args)
        {
            RELEASE_CHECK(!IsFull(), "Attempting to add to full circular buffer");

            new (m_data + TailIndex()) T(std::forward<Args>(args)...);
            ++m_tail;
        }

        T* Data()
        {
            return &m_data[0];
        }

        const T* Data() const
        {
            return &m_data[0];
        }

        size_t Size() const
        {
            return m_tail - m_head;
        }

        bool IsFull() const
        {
            return Size() == TCapacity;
        }

        bool IsEmpty() const
        {
            return Size() == 0;
        }

        T& Get(uint32_t index)
        {
            RELEASE_CHECK(index < Size(), "Circular buffer index out of bounds in Get()");
            return m_data[(HeadIndex() + index) % TCapacity];
        }

        const T& Get(uint32_t index) const
        {
            RELEASE_CHECK(index < Size(), "Circular buffer index out of bounds in Get()");
            return m_data[(HeadIndex() + index) % TCapacity];
        }

        const T& operator[](size_t index) const
        {
            return Get(index);
        }

        T& operator[](size_t index)
        {
            return Get(index);
        }

        void Clear()
        {
            for (size_t i = m_head; i < m_tail; ++i)
            {
                const size_t Index = i % TCapacity;
                T& object = m_data[i];
                object.~T();
            }

            m_size = 0;
            memset(m_data, 0, sizeof(m_data));
        }

    private:
        uint32_t TailIndex() const
        {
            return m_tail % TCapacity;
        }

        uint32_t HeadIndex() const
        {
            return m_head % TCapacity;
        }

        uint32_t m_head = 0;
        uint32_t m_tail = 0;

        T        m_data[TCapacity];
    };
}}