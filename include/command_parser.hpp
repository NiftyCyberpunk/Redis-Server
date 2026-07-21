# ifndef COMMAND_PARSER_HPP
# define COMMAND_PARSER_HPP
# include "command.hpp"
# include <string>



class CommandParser{
public:
    static Command parse(const std::string& input);
    //without static we need to make object first
    //this doesn't need to store any data
};

# endif