# include "aof.hpp"
# include "command.hpp"
#include "command_result.hpp"
# include "config.hpp"
# include "resp_parser.hpp"
# include <fstream>
# include <iterator>
#include <oaidl.h>

bool AOF::isReplaying = false;
//After this line we can remove the AOF:: from every isReplaying but it looks good to me...
std::ofstream AOF::file;

bool AOF::init(){

    if(!Config::appendOnly){
        return true;
    }

    file.open(Config::appendFile, std::ios::app);
    //std::ios::app ==> Don't overwrite the file append it...

    return file.is_open();
}

void AOF::close(){
    if(file.is_open()){
        file.close();
    }
}

bool AOF::append(const std::string &command){

    if(!Config::appendOnly || AOF::isReplaying){
        return true;
    }

    if(!file.is_open()){
        return true;
    }

    file <<command;

    return true;
}

bool AOF::replay(CommandHandler& handler){

    if(Config::appendOnly == false){
        return true;
    }

    AOF::isReplaying = true;

    std::ifstream file(Config::appendFile);

    if(!file){
        AOF::isReplaying = false;
        return true;
    }

    std::string data(
        (std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>()
    );
    //data contains entire aof

    while(!data.empty()){

        Command cmd = RespParser::parse(data);

        if(!cmd.valid){
            AOF::isReplaying = false;
            return false;
        }

        CommandResult result = handler.execute(cmd);

        if(result.type == ResultType::Error){
            AOF::isReplaying = false;
            return false;
        }

        data.erase(0, cmd.bytesConsumed);
    }

    AOF::isReplaying = false;

    return true;
}
