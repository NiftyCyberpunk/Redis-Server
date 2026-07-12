# include "protocol_formatter.hpp"
#include "command_result.hpp"

std::string ProtocolFormatter::formatter(const CommandResult &result){
    switch(result.type){

        case ResultType::SimpleString:
            return "+" + result.value;
        
        case ResultType::Integer:
            return ":" + result.value;
        
        case ResultType::BulkString:
            return "$" + result.value;
        
        case ResultType::Error:
            return "-ERROR " + result.value;
    }
    return "-ERROR Unknown result type";
}