#pragma once

#include <string>

namespace sputter { namespace core {
    class CommandLineArgument
    {
    public:
        explicit CommandLineArgument(const std::string& argumentName);
        CommandLineArgument(const std::string& argumentName, const std::string& argumentValue);

        const std::string& GetName() const;

        std::string AsString() const;

    private:
        const std::string m_name;
        const std::string m_value;
        bool m_isFlag = false;
    };
}}