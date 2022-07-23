#include "time.h"

#include <unistd.h>
#include <ctime>
#include <cerrno>

namespace sputter { namespace system {
    uint32_t GetTickMilliseconds()
    {
        struct timespec ts;
        unsigned tickMs = 0U;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        tickMs  = ts.tv_nsec / 1000000;
        tickMs += ts.tv_sec * 1000;

        return tickMs;
    }

    void SleepMs(uint32_t milliseconds)
    {
        struct timespec ts;

        ts.tv_sec  = milliseconds / 1000;
        ts.tv_nsec = (milliseconds % 1000) * 1000000;

        while (nanosleep(&ts, &ts) && errno == EINTR);
    }
}}
