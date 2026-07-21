# include "command_handler.hpp"
# include "command_result.hpp"
# include "database.hpp"
# include "persistance.hpp"
# include <cstddef>
# include <exception>
# include <string>
# include <vector>

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

    if(cmd.command == "TYPE"){
        return handleType(cmd);
    }

    if(cmd.command == "MGET"){
        return handleMget(cmd);
    }

    if(cmd.command == "MSET"){
        return handleMset(cmd);
    }

    if(cmd.command == "INCR"){
        return handleIncr(cmd);
    }

    if(cmd.command == "DECR"){
        return handleDecr(cmd);
    }

    if(cmd.command == "EXPIRE"){
        return handleExpire(cmd);
    }

    if(cmd.command == "TTL"){
        return handleTtl(cmd);
    }

    if(cmd.command == "PERSIST"){
        return handlePersist(cmd);
    }
    
    return {
        ResultType::Error,
        "Unknown command"
    };
}

void CommandHandler::cleanUpExpiredKey(){
    db.removeExpiredKey();
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

    //Persistance::saveToFile(db);

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
    bool removed = db.remove(key);

    if(removed) Persistance::saveToFile(db);

    return {
        ResultType::Integer,
        removed ? "1" : "0"
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
            "ECHO requires at least one argument"
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

    Persistance::saveToFile(db);

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

CommandResult CommandHandler::handleType(const Command& cmd){

    if(cmd.args.size() != 1){
        return {
            ResultType::Error,
            "TYPE requires one argument"
        };
    }
    return {
        ResultType::SimpleString,
        db.exists(cmd.args[0]) ? "string" : "none"
    };
}

CommandResult CommandHandler::handleRename(const Command& cmd){

    if(cmd.args.size() != 2){
        return {
            ResultType::Error,
            "RENAME requires two arguments"
        };
    }

    if(!db.exists(cmd.args[0])){
        return {
            ResultType::Error,
            "Old key not found"
        };
    }
    bool renamed = db.renameKey(cmd.args[0], cmd.args[1]);
    if(renamed) Persistance::saveToFile(db);

    return {
        ResultType::SimpleString,
        renamed ? "OK" : "Rename failed"
    };
}

CommandResult CommandHandler::handleMget(const Command& cmd){
    if(cmd.args.empty()){
        return {
            ResultType::Error,
            "MGET requires at least one argument"        
        };
    }

    std::vector<std::string> mget;

    for(const auto& key : cmd.args){
        auto value = db.get(key);

        if(!value){
            mget.push_back("");
            continue;
        }
        mget.push_back(*value);
    }
    return {
        ResultType::Array,
        "",
        mget        
    };
}

CommandResult CommandHandler::handleMset(const Command& cmd){
    if(cmd.args.empty()){
        return {
            ResultType::Error,
            "No key-value pairs to set"
        };
    }
    if(cmd.args.size() % 2 != 0){
        return {
            ResultType::Error,
            "Mset requires a key-value pairs"
        };
    }

    for(size_t i = 0; i < cmd.args.size() - 1; i += 2){
        std::string key = cmd.args[i];
        std::string value = cmd.args[i + 1];

        db.set(key, value);
    }

    Persistance::saveToFile(db);

    return {
        ResultType::SimpleString,
        "OK"
    };
}

CommandResult CommandHandler::handleIncr(const Command& cmd){
    if(cmd.args.size() != 1){
        return {
            ResultType::Error,
            "INCR require one argument"
        };
    }

    auto value = db.get(cmd.args[0]);
    
    if(!value){
        return {
            ResultType::Error,
            "Key not found"
        };
    }

    long long num;
    try{
        num = std::stoll(*value);
        if(std::to_string(num) != *value){
            return {
                ResultType::Error,
                "Value is not pure integer"
            };
        }

        db.set(cmd.args[0], std::to_string(num + 1));

        Persistance::saveToFile(db);

        return {
            ResultType::Integer,
            std::to_string(num + 1)
        };
    }
    catch(const std::exception&){
        return {
            ResultType::Error,
            "No integer value"
        };
    }
}

CommandResult CommandHandler::handleDecr(const Command& cmd){
    if(cmd.args.size() != 1){
        return {
            ResultType::Error,
            "DECR require one argument"
        };
    }

    auto value = db.get(cmd.args[0]);

    if(!value){
        return {
            ResultType::Error,
            "Key not found"
        };
    }
    long long num;
    try{
        num = std::stoll(*value);
        if(std::to_string(num) != *value){
            return {
                ResultType::Error,
                "Value is not pure integer"
            };
        }

        db.set(cmd.args[0], std::to_string(num - 1));

        Persistance::saveToFile(db);

        return {
            ResultType::Integer,
            std::to_string(num - 1)
        };
    }
    catch(const std::exception&){
        return {
            ResultType::Error,
            "No integer value"
        };
    }
}

CommandResult CommandHandler::handleExpire(const Command& cmd){

    if(cmd.args.size() != 2){
        return {
            ResultType::Error,
            "EXPIRE requires two arguments"
        };
    }

    auto key = cmd.args[0];

    if(!db.exists(key)){
        return {
            ResultType::Error,
            "Key not found"
        };
    }

    try{
        auto time = std::stoi(cmd.args[1]);

        if(std::to_string(time) != cmd.args[1]){
            return {
            ResultType::Error,
            "Second argument(time) must be integer value"
            };
        }
        bool isExpire = db.expire(key, time);

        if(isExpire){
            Persistance::saveToFile(db);
        }

        return {
            ResultType::Integer,
            isExpire ? "1" : "0"
        };
    }
    catch(const std::exception&){
        return {
            ResultType::Error,
            "Second argument(time) must be integer value"
        };
    }
}

CommandResult CommandHandler::handleTtl(const Command& cmd){
    if(cmd.args.size() != 1){
        return {
            ResultType::Error,
            "TTL require one argument"
        };
    }

    auto key = cmd.args[0];
    std::string ttl = std::to_string(db.ttl(key));

    return {
        ResultType::Integer,
        ttl
    };
}

CommandResult CommandHandler::handlePersist(const Command& cmd){
    if(cmd.args.size() != 1){
        return {
            ResultType::Error,
            "PERSIST require one argument"
        };
    }

    auto key = cmd.args[0];

    bool isPersist = db.persist(key);

    if(isPersist){
        Persistance::saveToFile(db);
    }

    return {
        ResultType::Integer,
        isPersist ? "1" : "0"
    };

}