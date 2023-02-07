#include "log.h"

#include <cstdio>
#include <cstdarg>
#include <cstdlib>

using namespace sputter;
using namespace sputter::log;

static LogVerbosity s_CurrentVerbosity = kDefaultLogVerbosity;
static const char* s_pLogFilePath = nullptr;
static bool ZoneMask[static_cast<uint8_t>(LogZone::Max)] = {true};
static FILE* s_pFile = stderr;

namespace 
{
    const char* ZoneToString(LogZone zone)
    {
        const uint8_t Index = static_cast<uint8_t>(zone);
        static const char* LUT[] = { 
            "Default",
            "Game"
        };

        return LUT[Index];
    }
}

void sputter::log::SetLogVerbosity(LogVerbosity newLogVerbosity)
{
    s_CurrentVerbosity = newLogVerbosity;
}

bool sputter::log::SetLogFile(const char* pLogFilePath)
{
    s_pLogFilePath = pLogFilePath;
    return false;
}

void sputter::log::DisableZone(LogZone zone)
{
    const uint8_t Index = static_cast<uint8_t>(zone);
    ZoneMask[Index] = false;
}

void sputter::log::EnableZone(LogZone zone)
{
    const uint8_t Index = static_cast<uint8_t>(zone);
    ZoneMask[Index] = true;
}

bool sputter::log::GetIsZoneEnabled(LogZone zone)
{
    const uint8_t Index = static_cast<uint8_t>(zone);
    return ZoneMask[Index];
}

void sputter::log::Log(LogZone zone, LogVerbosity verbosity, const char* pFormatString, ...)
{
    if (verbosity > s_CurrentVerbosity || !GetIsZoneEnabled(zone))
    {
        return;
    }

    char formatted[1024];

    va_list args;
    va_start(args, pFormatString);
    vsnprintf(formatted, sizeof(formatted), pFormatString, args);
    perror(formatted);
    va_end(args);

    fprintf(s_pFile, "%s: %s", ZoneToString(zone), formatted);
}