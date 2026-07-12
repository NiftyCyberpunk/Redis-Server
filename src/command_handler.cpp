# include "command_handler.hpp"
# include "command_parser.hpp"
#include "command_result.hpp"
# include "database.hpp"

CommandHandler::CommandHandler(Database& database) : db(database){

}

CommandResult CommandHandler::execute(const Command& cmd){

    std::string cmdName = cmd.command;
    std::string key;
    std::string data = "";

    if(cmdName == "SET"){
        if(cmd.args.empty()){
            return {
                ResultType::Error,
                "No arguments to SET"
            };
        }

        key = cmd.args[0];
        if(cmd.args.size() > 1){
            for(std::size_t i = 1; i < cmd.args.size(); i++){
                //std::size_t instead of int because cmd.args.size() returns std::size_t. This avoids signed/unsigned comparison warnings.
                if(i > 1){
                    data += " ";
                }
                data += cmd.args[i];
            }
        }

        if(data.empty()){
            return {
                ResultType::Error,
                "No data to set"
            };
        }

        db.set(key, data);
        return {
            ResultType::OK,
            ""
        };
    }

    if(cmdName == "GET"){
        if(cmd.args.size() != 1){
            return {
                ResultType::Error,
                "GET requires one argument"
            };
        }

        key = cmd.args[0];

        auto value = db.get(key);

        if(!value){
            return {
                ResultType::Error,
                "Key not found"
            };
        }

        return {
            ResultType::String,
            *value
        };
    }

    if(cmdName == "DEL"){
        if(cmd.args.size() != 1){
            return {
                ResultType::Error,
                "DEL requires one argument"
            };
        }

        key = cmd.args[0];

        return {
            ResultType::Integer,
            db.remove(key) ? "1" : "0"
        };
    }

    if(cmdName == "EXISTS"){
        if(cmd.args.size() != 1){
            return {
                ResultType::Error,
                "EXISTS requires one argument"
            };
        }

        key = cmd.args[0];

        return {
            ResultType::Integer,
            db.exists(key) ? "1" : "0"
        };
    }

    return {
        ResultType::Error,
        "Unknown Command"
    };
}