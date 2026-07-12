/*
    # ifndef COMMAND_RESULT_HPP
    # define COMMAND_RESULT_HPP

    # endif 
*/
//OR
# pragma once

# include <string>

enum class ResultType {
    SimpleString,
    Integer,
    Error,
    BulkString
};

struct CommandResult {
    ResultType type;
    std::string value;
};