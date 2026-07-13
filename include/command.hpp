# pragma once

# include <string>
# include <vector>

struct Command{
    std::string command;
    std::vector<std::string> args;

    bool valid = true;
    std::string errMsg;
};