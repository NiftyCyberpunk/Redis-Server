# include "protocol_formatter.hpp"
#include "command_result.hpp"
#include <cstddef>
#include <string>

std::string ProtocolFormatter::formatter(const CommandResult &result){
    switch(result.type){

        case ResultType::SimpleString:{
            return "+" + result.value;
        }
        case ResultType::Integer:{
            return ":" + result.value;
        }
        case ResultType::BulkString: {
            std::string length = std::to_string(result.value.length());
            return "$" + length + "\r\n" + result.value + '\n';
        }

        case ResultType::Array: {
            std::string length = std::to_string(result.list.size());
            std::string keys = "*" + length + "\r\n";

            for(size_t i = 0; i < result.list.size(); i++){

                const std::string& key = result.list[i];

                std::string keyLen = std::to_string(result.list[i].length());

                keys += "$" + keyLen + "\r\n" + key + "\n"; 
            }
            return keys;
        }

        case ResultType::Error:{
            return "-ERROR " + result.value;
        }
    }
    return "-ERROR Unknown result type";
}