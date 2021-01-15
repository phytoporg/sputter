#pragma once

#include <string>

namespace sputter { namespace system {
    // Initialize the logging system. programName is this executable name,
    // pLogFile is optional and redirects logging to a file path if not null. 
    void 
    InitializeLogging(
        const std::string& programName,
        const char* pLogFile = nullptr);

    // Throws a runtime exception and logs an error string.
    void LogAndFail(const std::string& errorString);
}}
