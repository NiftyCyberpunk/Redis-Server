# ifndef SERVER_HPP
# define SERVER_HPP
#include <WinSock2.h>
# include <winSock2.h>

class Server{
private:
    SOCKET serverSocket;
public:
    Server();
    ~Server();

    bool start();
};


#endif