# include "server.hpp"
#include "config.hpp"
# include "resp_parser.hpp"
# include "command_handler.hpp"
# include "command_result.hpp"
# include "protocol_formatter.hpp"
# include "logger.hpp"
# include <WinSock2.h>
# include <chrono>
#include <ole2.h>
# include <string>
#include <vector>
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

void Server::cleanUpExpiredKeys(){
    while(true){
        handler.cleanUpExpiredKey();

        std::this_thread::sleep_for(std::chrono::seconds(Config::cleanupInterval));
    }
}

void Server::handleClient(SOCKET clientSocket){


    ClientSession session;
    session.authenticated = Config::password.empty();

    Logger::info("Client connected");
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
            
            Command cmd = RespParser::parse(pendingData);

            if(cmd.valid){
                pendingData.clear();
                if(cmd.command == "QUIT"){
                    std::string endMsg = "+OK\r\n";
                    int byteSend = send(
                        clientSocket,
                        endMsg.data(),
                        static_cast<int>(endMsg.size()),
                        0
                    );
                    
                    if(byteSend == SOCKET_ERROR){
                        Logger::error("Send failed" + std::to_string(WSAGetLastError()));
                    }
                    isConnected = false;
                    break;
                }
                Logger::info("Command = [" + cmd.command + "]");
                if(cmd.command == "AUTH"){
                    if(cmd.args.size() != 1){
                        std::string response = "- AUTH require one argument\r\n";
                        send(
                            clientSocket,
                            response.data(),
                            static_cast<int>(response.size()),
                            0
                        );
                        continue;
                    }

                    std::string password = cmd.args[0];
                    if(password == Config::password){
                        session.authenticated = true;
                        std::string response = "+OK\r\n";
                        send(
                            clientSocket,
                            response.data(),
                            static_cast<int>(response.size()),
                            0
                        );
                    }
                    else{
                        std::string response = "- Invalid password\r\n";
                        send(
                            clientSocket,
                            response.data(),
                            static_cast<int>(response.size()),
                            0
                        );
                    }
                    continue;
                }

                if(cmd.command == "MULTI"){
                    if(session.inTransaction){
                        std::string response = "- MULTI calls cannot be nested\r\n";
                        send(
                            clientSocket,
                            response.data(),
                            static_cast<int>(response.size()),
                            0
                        );
                        continue;
                    }

                    session.inTransaction = true;

                    std::string response = "+OK\r\n";
                    send(
                        clientSocket,
                        response.data(),
                        static_cast<int>(response.size()),
                        0
                    );

                    continue;
                }

                if(cmd.command == "EXEC"){

                    if(!session.inTransaction){
                        std::string response = "- EXEC without MULTI\r\n";
                        send(
                            clientSocket,
                            response.data(),
                            static_cast<int>(response.size()),
                            0
                        );
                        continue;
                    }

                    std::vector<CommandResult> results;

                    for(const auto& command : session.queuedCommand){
                        results.push_back(handler.execute(command));
                    }

                    session.queuedCommand.clear();
                    session.inTransaction = false;

                    std::string response = ProtocolFormatter::formatTransactions(results);

                    send(
                        clientSocket,
                        response.data(),
                        static_cast<int>(response.size()),
                        0
                    );
                    continue;
                }

                if(cmd.command == "DISCARD"){
                    if(!session.inTransaction){
                        std::string response = "- DISCARD without MULTI\r\n";
                        send(
                            clientSocket,
                            response.data(),
                            static_cast<int>(response.size()),
                            0
                        );
                        continue;
                    }

                    session.queuedCommand.clear();
                    session.inTransaction = false;

                    std::string response = "+OK\r\n";

                    send(
                        clientSocket,
                        response.data(),
                        static_cast<int>(response.size()),
                        0
                    );
                    continue;
                }
                if(!session.authenticated && cmd.command != "QUIT" && cmd.command != "AUTH"){
                    std::string response = "- Authentication required\r\n";
                    send(
                        clientSocket,
                        response.data(),
                        static_cast<int>(response.size()),
                        0
                    );
                    continue;
                }
                if(session.inTransaction){

                    session.queuedCommand.push_back(cmd);
                    std::string response = "+QUEUED\r\n";

                    send(
                        clientSocket,
                        response.data(),
                        static_cast<int>(response.size()),
                        0
                    );
                    continue;
                }
                
                CommandResult result = handler.execute(cmd);

                std::string response = ProtocolFormatter::formatter(result);
                
                int bytesSend = send(
                    clientSocket,
                    response.data(),
                    static_cast<int>(response.size()),
                    0
                );
                
                if(bytesSend == SOCKET_ERROR){
                    Logger::error("Send failed" + std::to_string(WSAGetLastError()));
                    break;
                }
            }
        }
        else if(bytesReceived == 0){
            Logger::info("Client disconnected");
            break;
        }
        else{
            Logger::error("Receive failed" + std::to_string(WSAGetLastError()));
            break;
        }
        if(!isConnected){
            Logger::info("Client disconnected");
            break;
        }
    }
    closesocket(clientSocket);
}

bool Server::start(){
    WSAData wsadata;

    int result = WSAStartup(MAKEWORD(2, 2), &wsadata);

    if(result != 0){
        Logger::error("WSAStartup failed" + std::to_string(WSAGetLastError()));
        return false;
    }

    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    /*
        AF_INET = IPv4
        SOCK_STREAM = stream socket
        IPPROTO_TCP = protocol
    */

    if(serverSocket == INVALID_SOCKET){
        Logger::error("Socket creation failed" + std::to_string(WSAGetLastError()));
        WSACleanup();//releases/undoes the process’s successful Winsock initialization usage when you’re finished with Winsock.
        return false;
    }

    Logger::info("Socket created successfully");
    
    sockaddr_in serverAddress{};
    
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(Config::port);//htons --> host to network short
    //Network protocols use network byte order, so htons() converts the 16-bit port number into the required byte order.
    serverAddress.sin_addr.s_addr = INADDR_ANY;//Any IPv4 address

    int bindResult = bind(
        serverSocket,
        reinterpret_cast<sockaddr*>(&serverAddress),
        sizeof(serverAddress)
    );

    if(bindResult == SOCKET_ERROR){
        Logger::error("Bind failed" + std::to_string(WSAGetLastError()));
        closesocket(serverSocket);
        WSACleanup();
        return false;
    }
    
    Logger::info("Socket bound successfully");
    
    int listenResult = listen(
        serverSocket, 
        SOMAXCONN
    );

    if(listenResult == SOCKET_ERROR){
        Logger::error("Listen failed" + std::to_string(WSAGetLastError()));        
        closesocket(serverSocket);
        WSACleanup();
        return false;
    }

    Logger::info("Server listening on port " + std::to_string(Config::port));    
    
    
    Logger::info("Waiting for client");    

    cleanUpThread = std::thread(&Server::cleanUpExpiredKeys,this);
    cleanUpThread.detach();

    while (true){
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);

        if(clientSocket == INVALID_SOCKET){
        Logger::error("Accept failed" + std::to_string(WSAGetLastError()));
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