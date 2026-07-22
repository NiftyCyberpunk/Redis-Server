# ifndef COMMAND_HANDLER_HPP
# define COMMAND_HANDLER_HPP
# include "command.hpp"
# include "command_result.hpp"
# include "database.hpp"
# include "server_stats.hpp"

class CommandHandler{
private:
    Database& db;
    ServerStats& stats;

    CommandResult handleSet(const Command& cmd);
    CommandResult handleGet(const Command& cmd);
    CommandResult handleDel(const Command& cmd);
    CommandResult handleExists(const Command& cmd);
    CommandResult handlePing(const Command& cmd);
    CommandResult handleEcho(const Command& cmd);
    CommandResult handleKeys(const Command& cmd);
    CommandResult handleFlushDB(const Command& cmd);
    CommandResult handleDBsize(const Command& cmd);
    CommandResult handleType(const Command& cmd);
    CommandResult handleRename(const Command& cmd);
    CommandResult handleMget(const Command& cmd);
    CommandResult handleMset(const Command& cmd);
    CommandResult handleIncr(const Command& cmd);
    CommandResult handleDecr(const Command& cmd);
    CommandResult handleExpire(const Command& cmd);
    CommandResult handleTtl(const Command& cmd);
    CommandResult handlePersist(const Command& cmd);
    CommandResult handleInfo(const Command& cmd);
    CommandResult handleReplConf(const Command& cmd);
    CommandResult handlePsync(const Command& cmd);

    bool persistence(const Command& cmd);
public:
    CommandHandler(Database& database, ServerStats& stats);

    CommandResult execute(const Command& cmd);

    void cleanUpExpiredKey();
};

# endif