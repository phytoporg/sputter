#pragma once

#include <vector>

#include "cliargument.h"

namespace sputter { namespace core {
    class CommandLineArgumentParser
    {
    public:
        CommandLineArgumentParser(int argc, char** argv);

        const CommandLineArgument* FindArgument(const std::string& argumentName) const;

    private:
        std::vector<CommandLineArgument> m_parsedArguments;
    };
}}