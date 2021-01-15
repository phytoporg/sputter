#include "system.h"

#include <glog/logging.h>
#include <stdexcept>

namespace sputter { namespace system {
    void 
    InitializeLogging(
        const std::string& programName,
        const char* pLogFile)
    {
        if (pLogFile)
        {
            LOG(WARNING) << "Logging doesn't support redirection to files "
                         << "just yet.";
        }

        ::google::InitGoogleLogging(programName.c_str());
    }

    void LogAndFail(const std::string& errorString)
    {
        LOG(ERROR) << errorString;
        throw std::runtime_error(errorString);
    }
}}
