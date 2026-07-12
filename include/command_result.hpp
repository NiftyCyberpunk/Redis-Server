/*
    # ifndef COMMAND_RESULT_HPP
    # define COMMAND_RESULT_HPP

    # endif 
*/
//OR
# pragma once

# include <string>

enum class ResultType {
    OK,
    Integer,
    Error,
    String
};

struct CommandResult {
    ResultType type;
    std::string value;
};