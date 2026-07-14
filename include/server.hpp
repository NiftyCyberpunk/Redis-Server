# ifndef SERVER_HPP
# define SERVER_HPP
# include "command_handler.hpp"
# include <WinSock2.h>
# include <thread>
# include <winSock2.h>

class Server{
private:
    SOCKET serverSocket;//SOCKET is windows defined type to handles sockets
    CommandHandler& handler;
public:
    Server(CommandHandler& commandHandler);
    ~Server();
    
    void handleClient(SOCKET clientSocket);
    bool start();

    std::thread cleanUpThread;
    void cleanUpExpiredKeys();
};


#endif