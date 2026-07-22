# pragma once

# include "command_handler.hpp"
# include <WinSock2.h>
# include <string>

class ReplicaClient {
private:
    CommandHandler& handler;

    SOCKET masterSocket = INVALID_SOCKET;
    std::string receiveRaw();
    std::string sendAndReceive(const std::string& command);

public:
    ReplicaClient(CommandHandler& commandHandler);

    bool connectToMaster();
    bool start();
};