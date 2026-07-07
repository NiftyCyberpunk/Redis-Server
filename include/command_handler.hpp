# ifndef COMMAND_HANDLER_HPP
# define COMMAND_HANDLER_HPP
# include <string>
# include "command_parser.hpp"
# include "database.hpp"

class CommandHandler{
private:
    Database& db;
public:
    CommandHandler(Database& database);

    std::string execute(const Command& cmd);
};

#endif