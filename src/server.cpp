# include "server.hpp"
# include "command_parser.hpp"
# include "command_handler.hpp"
# include <WinSock2.h>
# include <cstddef>
# include <iostream>
# include <string>
# include <winSock2.h>
# include <thread>

Server::Server(CommandHandler& commandHandler):serverSocket(INVALID_SOCKET),handler(commandHandler){
    //No valid socket handled yet
}

Server::~Server(){
    if(serverSocket != INVALID_SOCKET){
        closesocket(serverSocket);
        WSACleanup();
    }
}

void Server::handleClient(SOCKET clientSocket){
    std::cout <<"Client connected\n";
    std::string pendingData;
    bool isConnected = true;
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

                std::cout <<"Received: "<<command<<'\n';

                pendingData.erase(0, pos + 1);

                Command cmd = CommandParser::parser(command);
                

                if(cmd.command == "EXIT" || cmd.command == "exit"){
                    std::string endMsg = "GoodBye...";
                    int byteSend = send(
                        clientSocket,
                        endMsg.data(),
                        static_cast<int>(endMsg.size()),
                        0
                    );
                    
                    if(byteSend == SOCKET_ERROR){
                        std::cerr <<"Send failed, Error: "<<WSAGetLastError()<<'\n';
                    }
                    isConnected = false;
                    break;
                }
    
                std::string response = handler.execute(cmd);

                response += '\n';
                
                int bytesSend = send(
                    clientSocket,
                    response.data(),
                    static_cast<int>(response.size()),
                    0
                );
                
                if(bytesSend == SOCKET_ERROR){
                    std::cerr <<"Send failed, Error: "<<WSAGetLastError()<<'\n';
                    break;
                }

            }
        }
        else if(bytesReceived == 0){
            std::cout <<"Client disconnected\n";
            break;
        }
        else{
            std::cerr <<"Receive failed, Error: "<<WSAGetLastError()<<'\n';
            break;
        }
        if(!isConnected){
            std::cout <<"Client disconnected\n";
            break;
        }
    }
    closesocket(clientSocket);
}

bool Server::start(){
    WSAData wsadata;

    int result = WSAStartup(MAKEWORD(2, 2), &wsadata);

    if(result != 0){
        std::cerr << "WSAStartup failed, Error: "<<WSAGetLastError()<<'\n';
        return false;
    }

    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    /*
        AF_INET = IPv4
        SOCK_STREAM = stream socket
        IPPROTO_TCP = protocol
    */

    if(serverSocket == INVALID_SOCKET){
        std::cerr <<"Socket creation failed, Error: "<<WSAGetLastError()<<'\n';
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
        std::cerr <<"Bind failed, Error: "<<WSAGetLastError()<<'\n';
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

    std::cout <<"Server listening on port 6379\n";
    

    std::cout <<"Waiting for client\n";

    while (true){
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);

        if(clientSocket == INVALID_SOCKET){
        std::cerr <<"Accept failed, Error: "<<WSAGetLastError()<<'\n';
        continue;;
    }

    std::thread clientThread(
        &Server::handleClient,
        this,
        clientSocket
    );

    clientThread.detach();
        
    }
    
    return true;
    
}