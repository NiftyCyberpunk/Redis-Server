# include "server.hpp"
# include "command_parser.hpp"
# include "command_handler.hpp"
# include <WinSock2.h>
# include <cstddef>
# include <inaddr.h>
# include <iostream>
# include <minwindef.h>
# include <oaidl.h>
# include <string>
# include <winSock2.h>
# include <winscard.h>

Server::Server(CommandHandler& commandHandler):serverSocket(INVALID_SOCKET),handler(commandHandler){
    //No valid socket handled yet
}

Server::~Server(){
    
}

bool Server::start(){
    WSAData wsadata;

    int result = WSAStartup(MAKEWORD(2, 2), &wsadata);

    if(result != 0){
        std::cerr << "WSAStartup failed\n";
        return false;
    }

    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    /*
        AF_INET = IPv4
        SOCK_STREAM = stream socket
        IPPROTO_TCP = protocol
    */

    if(serverSocket == INVALID_SOCKET){
        std::cerr <<"Socket creation failed\n";
        WSACleanup();//releases/undoes the process’s successful Winsock initialization usage when you’re finished with Winsock.
        return false;
    }

    std::cout <<"Socket created successfully\n";

    sockaddr_in serverAddress{};

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(6379);//htons --> host to network short
    //Network protocols use network byte order, so htons() converts the 16-bit port number into the required byte order.
    serverAddress.sin_addr.s_addr = INADDR_ANY;//Any IPv4 address

    int bindResult = bind(
        serverSocket,
        reinterpret_cast<sockaddr*>(&serverAddress),
        sizeof(serverAddress)
    );

    if(bindResult == SOCKET_ERROR){
        std::cerr <<"Bind failed\n";
        closesocket(serverSocket);
        WSACleanup();
        return false;
    }

    std::cout <<"Socket bound successfully\n";
    
    int listenResult = listen(
        serverSocket, 
        SOMAXCONN
    );

    if(listenResult == SOCKET_ERROR){
        std::cerr <<"Listen failed\n";
        closesocket(serverSocket);
        WSACleanup();
        return false;
    }

    std::cout <<"Server listning on port 6379\n";
    

    std::cout <<"Waiting for client\n";

    SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);

    if(clientSocket == INVALID_SOCKET){
        std::cerr <<"Accept failed\n";
        closesocket(serverSocket);
        WSACleanup();
        return false;
    }

    std::cout <<"Client connected\n";
    std::string pendingData;
    
    while(true){
        char buffer[1024];

        int bytesReceived = recv(
            clientSocket,
            buffer,
            sizeof(buffer) - 1,
            0
        );

        if(bytesReceived > 0){
            pendingData.append(buffer, bytesReceived);
            std::size_t pos;
            
            while((pos = pendingData.find('\n'))!= std::string::npos){
                std::string command = pendingData.substr(0, pos);

                std::cout <<"Recieved: "<<command<<'\n';

                pendingData.erase(0, pos + 1);

                Command cmd = CommandParser::parser(command);
    
                std::string response = handler.execute(cmd);

                response += '\n';
                
                int bytesSend = send(
                    clientSocket,
                    response.data(),
                    static_cast<int>(response.size()),
                    0
                );
                
                if(bytesSend == SOCKET_ERROR){
                    std::cerr <<"Send failed\n";
                    break;
                }

            }
        }
        else if(bytesReceived == 0){
            std::cout <<"Client disconnected\n";
            break;
        }
        else{
            std::cerr <<"Recieve failed\n";
            break;
        }
    }

    closesocket(clientSocket);
    
    return true;
    
}