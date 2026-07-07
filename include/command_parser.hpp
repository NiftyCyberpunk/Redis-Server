# ifndef COMMAND_PARSER_HPP
# define COMMAND_PARSER_HPP
# include <vector>
# include <string>

struct Command{
    std::string command;
    std::vector<std::string> args;
};

class CommandParser{
public:
    static Command parser(const std::string& input);
    //without static we need to make object first
    //this doesn't need to store any data
};

#endif