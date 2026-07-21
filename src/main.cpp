# include <iostream>
# include "aof.hpp"
# include "config.hpp"
# include "database.hpp"
# include "command_handler.hpp"
# include "persistence.hpp"
# include "pubsub.hpp"
# include "server.hpp"
# include "server_stats.hpp"

//# include <optional>

int main(){

    Config::load("config.cfg");

    if(!AOF::init()){
        std::cout <<"Failed to initialize AOF\n";
        return -1;
    }

    Database db;
    PubSub pubsub;
    ServerStats stats;

    stats.startTime = std::chrono::steady_clock::now();

    CommandHandler handler(db, stats);
    if (Config::appendOnly) {
        if (!AOF::replay(handler)) {
            std::cout << "Failed to replay AOF\n";
            return -1;
        }
    }
    else {
        Persistence::loadFromFile(db);
    }

    Server server(handler, pubsub, stats);

    if(!server.start()){
        std::cout <<"Server failed to start\n";
        return -1;
    }
    std::cout <<"Server started\n";

    return 0;
}