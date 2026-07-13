# include "command_parser.hpp"
# include <sstream>

Command CommandParser::parse(const std::string &input){

    Command cmd;

    std::stringstream ss(input);
    
    ss >> cmd.command;

    std::string arg;

    while(ss >> arg){
        cmd.args.push_back(arg);
    }

    return cmd;
}