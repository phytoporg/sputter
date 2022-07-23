#include "time.h"

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
