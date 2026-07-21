/*
    # ifndef COMMAND_RESULT_HPP
    # define COMMAND_RESULT_HPP

    # endif 
*/
//OR
# pragma once
# include <string>
# include <vector>

enum class ResultType {
    SimpleString,
    Integer,
    Error,
    BulkString,
    Array
};

struct CommandResult {
    ResultType type;
    std::string value;
    std::vector<std::string>list;
};