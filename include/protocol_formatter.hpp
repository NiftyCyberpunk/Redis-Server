# pragma once

#include "command_result.hpp"
#include <string>
#include <vector>
class ProtocolFormatter {
public:
    static std::string formatter(const CommandResult& result);
    static std::string formatTransactions(const std::vector<CommandResult> &results);
};