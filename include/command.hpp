# pragma once

# include <cstddef>
# include <string>
# include <vector>

struct Command{
    std::string command;
    std::vector<std::string> args;
    size_t bytesConsumed = 0;

    bool valid = true;
    std::string errMsg;
};