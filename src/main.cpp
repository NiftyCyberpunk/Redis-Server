# include <iostream>
# include "config.hpp"
# include "database.hpp"
# include "command_handler.hpp"
# include "persistance.hpp"
# include "pubsub.hpp"
# include "server.hpp"
# include "server_stats.hpp"
//# include <optional>

int main(){

    Config::load("config.cfg");

    Database db;
    PubSub pubsub;
    ServerStats stats;
    Persistance::loadFromFile(db);

    stats.startTime = std::chrono::steady_clock::now();

    CommandHandler handler(db, stats);
    Server server(handler, pubsub, stats);

    if(!server.start()){
        std::cout <<"Server failed to start\n";
        return -1;
    }
    std::cout <<"Server started\n";

    return 0;
}