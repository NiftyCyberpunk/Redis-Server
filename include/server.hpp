# ifndef SERVER_HPP
# define SERVER_HPP
# include "command_handler.hpp"
# include "pubsub.hpp"
# include <WinSock2.h>
# include <thread>
# include <vector>
# include <winSock2.h>

class Server{
private:
    std::mutex replicaMutex;

    SOCKET serverSocket;//SOCKET is windows defined type to handles sockets
    CommandHandler& handler;
    PubSub& pubsub;
    ServerStats& stats;
    struct ClientSession {
        bool inTransaction = false;
        std::vector<Command> queuedCommand;
        bool authenticated = false;
    };

    std::vector<SOCKET> replicaSockets;
public:
    Server(CommandHandler& commandHandler, PubSub& pubsub, ServerStats& stats);
    ~Server();
    
    void handleClient(SOCKET clientSocket);
    bool start();

    std::thread cleanUpThread;
    void cleanUpExpiredKeys();
    void sendAOF(SOCKET clientSocket);
    void broadcast(const std::string& command);
    bool isWriteCommand(const Command& cmd);
};


# endif