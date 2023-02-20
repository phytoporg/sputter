#include "system.h"

#include <sputter/log/log.h>
#include <stdexcept>

// LINUX++
#include <time.h>
#include <math.h>
// LINUX--

namespace sputter { namespace system {
    void 
    InitializeLogging(
        const std::string& programName,
        const char* pLogFile)
    {
        if (pLogFile)
        {
            sputter::log::SetLogFile(pLogFile);
        }

        // Enable all zones by default
        using namespace sputter::log;
        LogZone logZone = static_cast<LogZone>(0);
        while (logZone < LogZone::Max)
        {
            EnableZone(logZone);

            // Enum classes are gross
            logZone = static_cast<LogZone>(static_cast<int>(logZone) + 1);
        }
    }

    void LogAndFail(const std::string& errorString)
    {
        RELEASE_LOG_ERROR(LOG_DEFAULT, errorString.c_str());
        throw std::runtime_error(errorString);
    }

    uint32_t GetTimeMs()
    {
        struct timespec spec;
        clock_gettime(CLOCK_REALTIME, &spec);

        time_t seconds = spec.tv_sec;
        long milliseconds = round(spec.tv_nsec / 1.0e6);
        if (milliseconds > 999)
        {
            ++seconds;
            milliseconds = 0;
        }

        return seconds * 1000 + milliseconds;
    }
}}
