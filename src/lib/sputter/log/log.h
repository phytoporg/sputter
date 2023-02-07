#pragma once

#include <cstdint>

namespace sputter { namespace log {
    enum class LogVerbosity 
    {
        Error = 0,
        Warning,
        Info,
        Verbose,
        VeryVerbose,
        Max
    };
    static const LogVerbosity kDefaultLogVerbosity = LogVerbosity::Warning;

    enum class LogZone 
    {
        Default = 0,
        Game,
        Assets,
        UI,
        Max
    };

    // Log configuration
    void SetLogVerbosity(LogVerbosity newLogVerbosity);

    // Default is stderr
    bool SetLogFile(const char* pLogFilePath);

    // All zones enabled by default
    void DisableZone(LogZone zone);
    void EnableZone(LogZone zone);
    bool GetIsZoneEnabled(LogZone zone);

    void Log(LogZone zone, LogVerbosity verbosity, const char* pFormatString, ...);
}}

// Log macros

// Debug
#if defined(DEBUG)
    #define DEBUG_LOG_ERROR(Zone, String, ...) \
        sputter::log::Log(Zone, sputter::log::LogVerbosity::Error, String, __VA_ARGS__);
    #define DEBUG_LOG_WARNING(Zone, String, ...) \
        sputter::log::Log(Zone, sputter::log::LogVerbosity::Warning, String, __VA_ARGS__);
    #define DEBUG_LOG_INFO(Zone, String, ...) \
        sputter::log::Log(Zone, sputter::log::LogVerbosity::Info, String, __VA_ARGS__);
    #define DEBUG_LOG_VERBOSE(Zone, String, ...) \
        sputter::log::Log(Zone, sputter::log::LogVerbosity::Verbose, String, __VA_ARGS__);
    #define DEBUG_LOG_VERYVERBOSE(Zone, String, ...) \
        sputter::log::Log(Zone, sputter::log::LogVerbosity::VeryVerbose, String, __VA_ARGS__);
#else
    #define DEBUG_LOG_ERROR(Zone, String, ...) 
    #define DEBUG_LOG_WARNING(Zone, String, ...) 
    #define DEBUG_LOG_INFO(Zone, String, ...) 
    #define DEBUG_LOG_VERBOSE(Zone, String, ...) 
    #define DEBUG_LOG_VERYVERBOSE(Zone, String, ...) 
#endif

// Release
#define RELEASE_LOG_ERROR(Zone, String, ...) \
    sputter::log::Log(Zone, sputter::log::LogVerbosity::Error, String, __VA_ARGS__)
#define RELEASE_LOG_WARNING(Zone, String, ...) \
    sputter::log::Log(Zone, sputter::log::LogVerbosity::Warning, String, __VA_ARGS__)
#define RELEASE_LOG_INFO(Zone, String, ...) \
    sputter::log::Log(Zone, sputter::log::LogVerbosity::Info, String, __VA_ARGS__)
#define RELEASE_LOG_VERBOSE(Zone, String, ...) \
    sputter::log::Log(Zone, sputter::log::LogVerbosity::Verbose, String, __VA_ARGS__)
#define RELEASE_LOG_VERYVERBOSE(Zone, String, ...) \
    sputter::log::Log(Zone, sputter::log::LogVerbosity::VeryVerbose, String, __VA_ARGS__)

#define RELEASE_LOG_ERROR_(Zone, String, ...) \
    sputter::log::Log(Zone, sputter::log::LogVerbosity::Error, String)
#define RELEASE_LOG_WARNING_(Zone, String, ...) \
    sputter::log::Log(Zone, sputter::log::LogVerbosity::Warning, String)
#define RELEASE_LOG_INFO_(Zone, String, ...) \
    sputter::log::Log(Zone, sputter::log::LogVerbosity::Info, String)
#define RELEASE_LOG_VERBOSE_(Zone, String, ...) \
    sputter::log::Log(Zone, sputter::log::LogVerbosity::Verbose, String)
#define RELEASE_LOG_VERYVERBOSE_(Zone, String, ...) \
    sputter::log::Log(Zone, sputter::log::LogVerbosity::VeryVerbose, String)

#define LOG_DEFAULT sputter::log::LogZone::Default
#define LOG_GAME sputter::log::LogZone::Game
#define LOG_ASSETS sputter::log::LogZone::Assets
#define LOG_UI sputter::log::LogZone::UI