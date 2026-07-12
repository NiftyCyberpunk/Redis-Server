# pragma once

#include "command_result.hpp"
#include <string>
class ProtocolFormatter {
public:
    static std::string formatter(const CommandResult& result);
};