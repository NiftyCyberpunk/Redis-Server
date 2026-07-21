# include "resp_parser.hpp"
# include <cstddef>
# include <string>

Command RespParser::parse(const std::string& input){
    Command cmd;

    if(input.empty() || input[0] != '*'){
        return {
            .valid = false,
            .errMsg = "Invalid RESP msg"
        };
    }

    size_t lenEnd = input.find("\r\n");
    if(lenEnd == std::string::npos){
        return {
            .valid = false,
            .errMsg = "Invalid RESP msg"
        };
    }

    int arrLen = std::stoi(input.substr(1, lenEnd - 1));

    size_t pos = input.find('$', lenEnd + 2);
    if(pos == std::string::npos){
        return {
            .valid = false,
            .errMsg = "Invalid RESP msg"
        };
    }

    size_t end = input.find("\r\n", pos);
    if(end == std::string::npos){
        return {
            .valid = false,
            .errMsg = "Invalid RESP msg"
        };
    }

    int bulkLength = std::stoi(input.substr(pos + 1, end - pos - 1));

    size_t commandStart = end + 2;

    if(commandStart + bulkLength + 2 > input.size()){
        return {
            .valid = false
        };
    }

    cmd.command = input.substr(commandStart, bulkLength);

    size_t current = commandStart + bulkLength + 2;
    // while(current < input.bulkLength()){
    for(int i = 1; i < arrLen; i++){
        
        size_t dollar = input.find('$', current);
        if(dollar == std::string::npos){
            return {
                .valid = false,
                .errMsg = "Invalid RESP msg"
            };
        }

        size_t lineEnd = input.find("\r\n", dollar);
        if(lineEnd == std::string::npos){
            return {
                .valid = false,
                .errMsg = "Invalid RESP msg"
            };
        }

        bulkLength = std::stoi(input.substr(dollar + 1, lineEnd - dollar - 1));
        size_t argStart = lineEnd + 2;

        if(argStart + bulkLength + 2 > input.size()){
            return {
                .valid = false
            };
        }

        cmd.args.push_back(input.substr(argStart ,bulkLength));

        current = argStart + bulkLength + 2;
    }
    cmd.bytesConsumed = current;

    return cmd;
}