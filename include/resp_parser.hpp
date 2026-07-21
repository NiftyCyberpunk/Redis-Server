# pragma once

# include "command.hpp"

class RespParser{
public:
    static Command parse(const std::string& input);
};