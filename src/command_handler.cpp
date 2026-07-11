# include "command_handler.hpp"
# include "command_parser.hpp"
# include "database.hpp"

CommandHandler::CommandHandler(Database& database) : db(database){

}

std::string CommandHandler::execute(const Command& cmd){

    std::string cmdName = cmd.command;
    std::string key;
    std::string data = "";

    if(cmdName == "SET"){
        if(cmd.args.empty()){
            return "-ERROR No arguments to SET...";
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
            return "-ERROR No data to set...";
        }

        db.set(key, data);
        return "+OK";
    }

    if(cmdName == "GET"){
        if(cmd.args.size() != 1){
            return "-ERROR GET requires one argument...";
        }

        key = cmd.args[0];

        auto value = db.get(key);

        if(!value){
            return "-ERROR Key not found...";
        }

        return "$" +  *value;
    }

    if(cmdName == "DEL"){
        if(cmd.args.size() != 1){
            return "-ERROR DEL requires one argument...";
        }

        key = cmd.args[0];

        return db.remove(key) ? ":1" : ":0";
    }

    if(cmdName == "EXISTS"){
        if(cmd.args.size() != 1){
            return "-ERROR EXISTS requires one argument...";
        }

        key = cmd.args[0];

        return db.exists(key) ? ":1" : ":0";
    }

    return "-ERROR Unknown Command...";
}