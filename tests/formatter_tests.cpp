# include "command_result.hpp"
# include "protocol_formatter.hpp"
# include "tests_utils.hpp"
# include <string>

void testSimpleString(){
    CommandResult result {
        ResultType::SimpleString,
        "OK"
    };

    std::string response = ProtocolFormatter::formatter(result);

    Utils::msgPrint((response == "+OK"), "SimpleString");
    
}
void testBulkString(){
    CommandResult result {
        ResultType::BulkString,
        "Nifty"
    };

    std::string response = ProtocolFormatter::formatter(result);

    Utils::msgPrint((response == "$5\r\nNifty\n"), "Bulkstring");
}
void testInteger(){
    CommandResult result {
        ResultType::Integer,
        "1"
    };

    std::string response = ProtocolFormatter::formatter(result);

    Utils::msgPrint((response == ":1"), "Integer");
}
void testError(){
    CommandResult result {
        ResultType::Error,
        "Something went wrong"
    };

    std::string response = ProtocolFormatter::formatter(result);

    Utils::msgPrint((response == "-ERROR Something went wrong"), "Error");
}