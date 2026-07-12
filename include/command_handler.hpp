# ifndef COMMAND_HANDLER_HPP
# define COMMAND_HANDLER_HPP
# include "command_parser.hpp"
#include "command_result.hpp"
# include "database.hpp"

class CommandHandler{
private:
    Database& db;
public:
    CommandHandler(Database& database);

    CommandResult execute(const Command& cmd);
};

#endif