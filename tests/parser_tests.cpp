# include "command_parser.hpp"
# include "tests_utils.hpp"
# include <string>
# include <vector>

void testSetParsing(){
    std::string input = "SET name Aryan";
    std::vector<std::string> args = {"name", "Aryan"}; 

    Command cmd = CommandParser::parse(input);

    Utils::msgPrint((cmd.command == "SET" && cmd.args == args), "SetParsing");
}
void testGetParsing(){
    std::string input = "GET name";

    Command cmd = CommandParser::parse(input);


    bool condition = (cmd.command == "GET") && (cmd.args.size() == 1);

    Utils::msgPrint(condition, "GetParsing");
}
void testInvalidCommand(){
    std::string input = "SET name";

    Command cmd = CommandParser::parse(input);

    Utils::msgPrint((cmd.command == "SET" && !cmd.args.empty()), "InvalidCommand");
}