# include <iostream>
# include "config.hpp"
# include "database.hpp"
# include "command_handler.hpp"
# include "persistance.hpp"
# include "pubsub.hpp"
# include "server.hpp"
//# include <optional>

int main(){

    Config::load("config.cfg");

    Database db;
    PubSub pubsub;
    Persistance::loadFromFile(db);
    CommandHandler handler(db);
    
    Server server(handler, pubsub);

    if(!server.start()){
        std::cout <<"Server failed to start\n";
        return -1;
    }
    std::cout <<"Server started\n";

    return 0;
}