#include "system.h"

#include <glog/logging.h>
#include <sputter/log/log.h>
#include <stdexcept>

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
    }

    void LogAndFail(const std::string& errorString)
    {
        RELEASE_LOG_ERROR_(LOG_DEFAULT, errorString.c_str());
        throw std::runtime_error(errorString);
    }
}}
