# include "command_handler.hpp"
# include "command_parser.hpp"
#include "command_result.hpp"
# include "database.hpp"
#include <cstddef>
#include <string>
#include <vector>

CommandHandler::CommandHandler(Database& database) : db(database){

}

CommandResult CommandHandler::execute(const Command& cmd){

    std::string cmdName = cmd.command;

    if(cmd.command == "SET"){
        return handleSet(cmd);
    }

    if(cmd.command == "GET"){
        return handleGet(cmd);
    }

    if(cmd.command == "DEL"){
        return handleDel(cmd);
    }

    if(cmd.command == "EXISTS"){
        return handleExists(cmd);
    }

    if(cmd.command == "PING"){
        return handlePing(cmd);
    }

    if(cmd.command == "ECHO"){
        return handleEcho(cmd);
    }

    if(cmd.command == "KEYS"){
        return handleKeys(cmd);
    }

    if(cmd.command == "FLUSHDB"){
        return handleFlushDB(cmd);
    }

    if(cmd.command == "DBSIZE"){
        return handleDBsize(cmd);
    }
    
    return {
        ResultType::Error,
        "Unknown command"
    };
}

CommandResult CommandHandler::handleSet(const Command& cmd){
    std::string key;
    std::string data = "";

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
        ResultType::SimpleString,
        "OK"
    };
}

CommandResult CommandHandler::handleGet(const Command& cmd){

    std::string key;
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
        ResultType::BulkString,
        *value
    };
}

CommandResult CommandHandler::handleDel(const Command& cmd){

    std::string key;
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

CommandResult CommandHandler::handleExists(const Command& cmd){

    std::string key;
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

CommandResult CommandHandler::handlePing(const Command& cmd){

    if(!cmd.args.empty()){
        return {
            ResultType::Error,
            "PING requires no arguments"
        };
    }
    return {
        ResultType::SimpleString,
        "PONG"
    };
}

CommandResult CommandHandler::handleEcho(const Command& cmd){

    if(cmd.args.empty()){
        return {
            ResultType::Error,
            "No argruments present"
        };
    }

    std::string input = "";
    for(size_t i = 0; i < cmd.args.size(); i++){
        if(i > 0){
            input += " ";
        }
        input += cmd.args[i];
    }

    return {
        ResultType::BulkString,
        input
    };
}

CommandResult CommandHandler::handleKeys(const Command& cmd){

    if(!cmd.args.empty()){
        return {
            ResultType::Error,
            "KEYS require no arguments"
        };
    }
    const auto keys = db.getKeys();

    // std::string keysStr;

    // for(size_t i = 0; i < keys.size(); i++){
    //     if(i > 0){
    //         keysStr += " ";
    //     }
    //     keysStr += keys[i];
    // }
    return {
        ResultType::Array,
        "",
        keys
    };
}

CommandResult CommandHandler::handleFlushDB(const Command& cmd){

    if(!cmd.args.empty()){
        return {
            ResultType::Error,
            "FLUSHDB requires no arguments"
        };
    }
    db.clear();
    return {
        ResultType::SimpleString,
        "OK"
    };
}

CommandResult CommandHandler::handleDBsize(const Command& cmd){

    if(!cmd.args.empty()){
        return {
            ResultType::Error,
            "DBSIZE requires no arguments"
        };
    }
    return {
        ResultType::Integer,
        std::to_string(db.size())
    };
}