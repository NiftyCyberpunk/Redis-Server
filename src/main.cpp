# include "aof.hpp"
# include "config.hpp"
# include "database.hpp"
# include "command_handler.hpp"
# include "persistence.hpp"
# include "pubsub.hpp"
# include "replica_client.hpp"
# include "server.hpp"
# include "server_stats.hpp"
# include "logger.hpp"
# include <WinSock2.h>
# include <iostream>
# include <string>
# include <thread>

//# include <optional>

int main(int argc, char* argv[]){

    WSAData wsadata;

    int result = WSAStartup(MAKEWORD(2, 2), &wsadata);

    if(result != 0){
        Logger::error("WSAStartup failed" + std::to_string(WSAGetLastError()));
        return -1;
    }

    std::string configFile = "config.cfg";

    if(argc > 1){
        configFile = argv[1];
    }

    Config::load(configFile);

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

    if(Config::isReplica){
        std::thread replicaThread([&]{
            ReplicaClient replica(handler);
            replica.start();
        });
        replicaThread.detach();
    }

    if(!server.start()){
        std::cout <<"Server failed to start\n";
        return -1;
    }
    std::cout <<"Server started\n";

    WSACleanup();
    return 0;
}