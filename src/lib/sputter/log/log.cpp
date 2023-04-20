#include "log.h"

#include <sputter/core/check.h>

#include <cstdio>
#include <cstdarg>
#include <cstdlib>
#include <cstring>

using namespace sputter;
using namespace sputter::log;

static LogVerbosity s_CurrentVerbosity = kDefaultLogVerbosity;
static bool ZoneMask[static_cast<uint8_t>(LogZone::Max)] = {};
static FILE* s_pFile = stderr;

namespace 
{
    const char* ZoneToString(LogZone zone)
    {
        const uint8_t Index = static_cast<uint8_t>(zone);
        static const char* LUT[] = { 
            "Default",
            "Game",
            "Assets",
            "UI",
            "Font",
            "Mesh",
            "Render",
            "Net",
            "FrameState",
        };

        static_assert(sizeof(LUT) / sizeof(LUT[0]) == static_cast<int>(LogZone::Max), "Log zone LUT size does not match enum");

        return LUT[Index];
    }
}

void sputter::log::SetLogVerbosity(LogVerbosity newLogVerbosity)
{
    s_CurrentVerbosity = newLogVerbosity;
}

void SetLogVerbosityFromString(const char* pVerbosityString)
{
    RELEASE_CHECK(pVerbosityString, "pVerbosityString cannot be null");

    // const uint8_t Index = static_cast<uint8_t>(zone);
    static const char* LUT[] = {
            "Error",
            "Warning",
            "Info",
            "Verbose",
            "VeryVerbose",
    };

    static_assert(
        sizeof(LUT) / sizeof(LUT[0]) == static_cast<int>(LogVerbosity::Max),
        "Log verbosity LUT size does not match enum");

    for (size_t i = 0; i < static_cast<int>(LogVerbosity::Max); ++i)
    {
        // TODO: case-insensitive search
        if (strncmp(LUT[i], pVerbosityString, sizeof(LUT[i])) == 0)
        {
            SetLogVerbosity(static_cast<LogVerbosity>(i));
            return;
        }
    }

    RELEASE_LOGLINE_WARNING(LOG_DEFAULT, "Failed to find verbosity string: %s", pVerbosityString);
}

bool sputter::log::SetLogFile(const char* pLogFilePath)
{
    if (s_pFile != stderr && s_pFile != stdout)
    {
        fclose(s_pFile);
        s_pFile = nullptr;
    }

    FILE* pFile = fopen(pLogFilePath, "w");
    if (!pFile) { return false; }

    s_pFile = pFile;
    return true;
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

void sputter::log::LogLine(LogZone zone, LogVerbosity verbosity, const char* pFormatString, ...)
{
    if (verbosity > s_CurrentVerbosity || !GetIsZoneEnabled(zone))
    {
        return;
    }

    char formatted[1024];

    va_list args;
    va_start(args, pFormatString);
    vsnprintf(formatted, sizeof(formatted), pFormatString, args);
    va_end(args);

    fprintf(s_pFile, "%s: %s\n", ZoneToString(zone), formatted);
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
    va_end(args);

    fprintf(s_pFile, "%s: %s", ZoneToString(zone), formatted);
}

void sputter::log::LogFlush()
{
    fflush(s_pFile);
}
