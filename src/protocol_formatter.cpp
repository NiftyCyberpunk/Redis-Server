# include "protocol_formatter.hpp"
#include "command_result.hpp"
#include <cstddef>
#include <string>

std::string ProtocolFormatter::formatter(const CommandResult &result){
    switch(result.type){

        case ResultType::SimpleString:{
            return "+" + result.value + "\r\n";
        }
        case ResultType::Integer:{
            return ":" + result.value + "\r\n";
        }
        case ResultType::BulkString: {
            std::string length = std::to_string(result.value.length());
            return "$" + length + "\r\n" + result.value + "\r\n";
        }

        case ResultType::Array: {
            std::string length = std::to_string(result.list.size());
            std::string keys = "*" + length + "\r\n";

            for(size_t i = 0; i < result.list.size(); i++){

                const std::string& key = result.list[i];

                std::string keyLen = std::to_string(result.list[i].length());

                keys += "$" + keyLen + "\r\n" + key + "\r\n"; 
            }
            return keys;
        }

        case ResultType::Error:{
            return "-ERROR " + result.value + "\r\n";
        }
    }
    return "-ERROR Unknown result type";
}