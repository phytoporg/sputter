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
        Font,
        Mesh,
        Render,
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

    void LogLine(LogZone zone, LogVerbosity verbosity, const char* pFormatString, ...);
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

    #define DEBUG_LOGLINE_ERROR(Zone, String, ...) \
        sputter::log::LogLine(Zone, sputter::log::LogVerbosity::Error, String, __VA_ARGS__);
    #define DEBUG_LOGLINE_WARNING(Zone, String, ...) \
        sputter::log::LogLine(Zone, sputter::log::LogVerbosity::Warning, String, __VA_ARGS__);
    #define DEBUG_LOGLINE_INFO(Zone, String, ...) \
        sputter::log::LogLine(Zone, sputter::log::LogVerbosity::Info, String, __VA_ARGS__);
    #define DEBUG_LOGLINE_VERBOSE(Zone, String, ...) \
        sputter::log::LogLine(Zone, sputter::log::LogVerbosity::Verbose, String, __VA_ARGS__);
    #define DEBUG_LOGLINE_VERYVERBOSE(Zone, String, ...) \
        sputter::log::LogLine(Zone, sputter::log::LogVerbosity::VeryVerbose, String, __VA_ARGS__);
#else
    #define DEBUG_LOG_ERROR(Zone, String, ...) 
    #define DEBUG_LOG_WARNING(Zone, String, ...) 
    #define DEBUG_LOG_INFO(Zone, String, ...) 
    #define DEBUG_LOG_VERBOSE(Zone, String, ...) 
    #define DEBUG_LOG_VERYVERBOSE(Zone, String, ...) 

    #define DEBUG_LOGLINE_ERROR(Zone, String, ...) 
    #define DEBUG_LOGLINE_WARNING(Zone, String, ...) 
    #define DEBUG_LOGLINE_INFO(Zone, String, ...) 
    #define DEBUG_LOGLINE_VERBOSE(Zone, String, ...)
    #define DEBUG_LOGLINE_VERYVERBOSE(Zone, String, ...)
#endif

// Release
#define RELEASE_LOG_ERROR(Zone, String, ...) \
    sputter::log::Log(Zone, sputter::log::LogVerbosity::Error, String __VA_OPT__(,) __VA_ARGS__)
#define RELEASE_LOG_WARNING(Zone, String, ...) \
    sputter::log::Log(Zone, sputter::log::LogVerbosity::Warning, String __VA_OPT__(,) __VA_ARGS__)
#define RELEASE_LOG_INFO(Zone, String, ...) \
    sputter::log::Log(Zone, sputter::log::LogVerbosity::Info, String __VA_OPT__(,) __VA_ARGS__)
#define RELEASE_LOG_VERBOSE(Zone, String, ...) \
    sputter::log::Log(Zone, sputter::log::LogVerbosity::Verbose, String __VA_OPT__(,) __VA_ARGS__)
#define RELEASE_LOG_VERYVERBOSE(Zone, String, ...) \
    sputter::log::Log(Zone, sputter::log::LogVerbosity::VeryVerbose, String __VA_OPT__(,) __VA_ARGS__)

#define RELEASE_LOGLINE_ERROR(Zone, String, ...) \
    sputter::log::LogLine(Zone, sputter::log::LogVerbosity::Error, String __VA_OPT__(,) __VA_ARGS__)
#define RELEASE_LOGLINE_WARNING(Zone, String, ...) \
    sputter::log::LogLine(Zone, sputter::log::LogVerbosity::Warning, String __VA_OPT__(,) __VA_ARGS__)
#define RELEASE_LOGLINE_INFO(Zone, String, ...) \
    sputter::log::LogLine(Zone, sputter::log::LogVerbosity::Info, String __VA_OPT__(,) __VA_ARGS__)
#define RELEASE_LOGLINE_VERBOSE(Zone, String, ...) \
    sputter::log::LogLine(Zone, sputter::log::LogVerbosity::Verbose, String __VA_OPT__(,) __VA_ARGS__)
#define RELEASE_LOGLINE_VERYVERBOSE(Zone, String, ...) \
    sputter::log::LogLine(Zone, sputter::log::LogVerbosity::VeryVerbose, String __VA_OPT__(,) __VA_ARGS__)

#define LOG_DEFAULT sputter::log::LogZone::Default
#define LOG_GAME sputter::log::LogZone::Game
#define LOG_ASSETS sputter::log::LogZone::Assets
#define LOG_UI sputter::log::LogZone::UI
#define LOG_FONT sputter::log::LogZone::Font
#define LOG_MESH sputter::log::LogZone::Mesh
#define LOG_RENDER sputter::log::LogZone::Render
