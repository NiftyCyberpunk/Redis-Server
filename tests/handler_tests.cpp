#include "command_handler.hpp"
#include "command_parser.hpp"
#include "command_result.hpp"
# include "database.hpp"
# include "tests_utils.hpp"

void testSetCommand(){
    Database db;
    CommandHandler handler(db);

    Command cmd;
    cmd.command = "SET";
    cmd.args.push_back("name");
    cmd.args.push_back("Aryan");


    CommandResult result = handler.execute(cmd);
    
    Utils::msgPrint((result.type == ResultType::SimpleString && result.value == "OK"), "SetCommand");

    auto value = db.get("name");

    Utils::msgPrint(value && *value == "Aryan","SetCommand stores value");
}
void testGetCommand(){
    Database db;
    CommandHandler handler(db);

    db.set("name", "Aryan");

    Command cmd;
    cmd.command = "GET";
    cmd.args.push_back("name");

    CommandResult result = handler.execute(cmd);
    
    Utils::msgPrint((result.type == ResultType::BulkString && result.value == "Aryan"), "GetCommand");
}
void testPingCommand(){
    Database db;
    CommandHandler handler(db);

    Command cmd;
    cmd.command = "PING";

    CommandResult result = handler.execute(cmd);

    Utils::msgPrint((result.type == ResultType::SimpleString && result.value == "PONG"), "PingCommand");
}

void testGetInvalidArguments(){
    Database db;
    CommandHandler handler(db);

    Command cmd;
    cmd.command = "GET";

    CommandResult result = handler.execute(cmd);

    Utils::msgPrint(result.type == ResultType::Error,"GET invalid arguments");
}