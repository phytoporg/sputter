#include "time.h"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
// TODO: #include some other stuff
#else
#include <unistd.h>
#endif

#include <ctime>
#include <cerrno>

namespace sputter { namespace system {
    uint32_t GetTickMilliseconds()
    {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
        // Not yet implemented
        return 0;
#else
        struct timespec ts;
        unsigned tickMs = 0U;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        tickMs  = ts.tv_nsec / 1000000;
        tickMs += ts.tv_sec * 1000;

        return tickMs;
#endif
    }

    void SleepMs(uint32_t milliseconds)
    {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
        // Not yet implemented
#else
        struct timespec ts;

        ts.tv_sec  = milliseconds / 1000;
        ts.tv_nsec = (milliseconds % 1000) * 1000000;

        while (nanosleep(&ts, &ts) && errno == EINTR);
#endif
    }
}}
