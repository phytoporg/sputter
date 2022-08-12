#pragma once

#include <sputter/system/system.h>

// For serializing pointers directly in relocatable PODs
namespace sputter { namespace memory {
    template<typename T>
    class SPtr {
    public:
        static void SetRegionBounds(void* pRegionBase, size_t regionSize)
        {
            s_regionBase = pRegionBase;
            s_regionSize = regionSize;
        }

        SPtr(T* ptr)
        {
            m_regionOffset = static_cast<void*>(ptr) - s_regionBase;
            DebugValidate();
        }

        T* operator=(T* ptr)
        {
            m_regionOffset = static_cast<void*>(ptr) - s_regionBase;
            DebugValidate();
        }

        T& operator*()
        {
            T* pTyped = static_cast<T*>(s_regionBase + m_regionOffset);
            return *pTyped;
        }

        T* operator->()
        {
            T* pTyped = static_cast<T*>(s_regionBase + m_regionOffset);
            return pTyped;
        }

    private:
        void DebugValidate()
        {
#if DEBUG
            if (m_regionOffset < 0 || (m_regionOffset + sizeof(T) > s_regionBase + s_regionSize))
            {
                sputter::system::LogAndFail("SPtr assignment is out of range");
            }
#endif
        }

        size_t m_regionOffset;

        static void* s_regionBase = nullptr;
        static size_t s_regionSize = 0;
    };
}}