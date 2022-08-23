#include "time.h"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define NOMINMAX
#endif

#include <windows.h>

namespace sputter { namespace system {
    uint32_t GetTickMilliseconds()
    {
        return static_cast<uint32_t>(GetTickCount());
    }

    void SleepMs(uint32_t milliseconds)
    {
        Sleep(milliseconds);
    }
}}
