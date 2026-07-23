# include "server.hpp"
# include "config.hpp"
# include "pubsub.hpp"
# include "resp_parser.hpp"
# include "command_handler.hpp"
# include "command_result.hpp"
# include "protocol_formatter.hpp"
# include "logger.hpp"
# include "server_stats.hpp"
# include <WinSock2.h>
# include <chrono>
# include <fstream>
# include <iterator>
# include <mutex>
# include <ole2.h>
# include <string>
# include <vector>
# include <winSock2.h>
# include <thread>

Server::Server(CommandHandler& commandHandler, PubSub& pubsub, ServerStats& stats):serverSocket(INVALID_SOCKET),handler(commandHandler),pubsub(pubsub), stats(stats){
    //No valid socket handled yet
}

Server::~Server(){
    if(serverSocket != INVALID_SOCKET){
        closesocket(serverSocket);
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
            
            while(!pendingData.empty()){
                Command cmd = RespParser::parse(pendingData);

                if (!cmd.valid) {
                    break;
                }

                pendingData.erase(0, cmd.bytesConsumed);
                    
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
                //Logger::info("Command = [" + cmd.command + "]");
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
                        stats.totalCommands++;
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

                stats.totalCommands++;

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

                        if(isWriteCommand(command)){
                            broadcast(cmd.raw);
                        }
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

                if(cmd.command == "SUBSCRIBE"){
                    if(cmd.args.size() != 1){
                        std::string response = "- SUBSCRIBE require one argument\r\n";
                        send(
                            clientSocket,
                            response.data(),
                            static_cast<int>(response.size()),
                            0
                        );
                        continue;
                    }

                    pubsub.subscribe(cmd.args[0], clientSocket);
                    std::string response = "+OK\r\n";
                    send(
                        clientSocket,
                        response.data(),
                        static_cast<int>(response.size()),
                        0
                    );
                    continue;
                }

                if(cmd.command == "UNSUBSCRIBE"){
                    if(cmd.args.size() != 1){
                        std::string response = "- UNSUBSCRIBE require one argument\r\n";
                        send(
                            clientSocket,
                            response.data(),
                            static_cast<int>(response.size()),
                            0
                        );
                        continue;
                    }

                    pubsub.unsubscribe(cmd.args[0], clientSocket);
                    std::string response = "+OK\r\n";
                    send(
                        clientSocket,
                        response.data(),
                        static_cast<int>(response.size()),
                        0
                    );
                    continue;
                }

                if(cmd.command == "PUBLISH"){
                    if(cmd.args.size() < 2){
                        std::string response = "- PUBLISH require atleast two arguments\r\n";
                        send(
                            clientSocket,
                            response.data(),
                            static_cast<int>(response.size()),
                            0
                        );
                        continue;
                    }


                    std::string message = "";

                    for(int i = 1; i < cmd.args.size(); i++){
                        if(i > 1){
                            message += " ";
                        }
                        message += cmd.args[i];
                    }

                    int delivered = pubsub.publish(cmd.args[0], message);
                    
                    CommandResult result{
                        ResultType::Integer,
                        std::to_string(delivered)
                    };


                    std::string response = ProtocolFormatter::formatter(result);
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

                if(isWriteCommand(cmd)){
                    broadcast(cmd.raw);
                }

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

                if(cmd.command == "PSYNC"){
                    sendAOF(clientSocket);
                    std::lock_guard<std::mutex> lock(replicaMutex);
                    replicaSockets.push_back(clientSocket);
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
    stats.connectedClients--;
    pubsub.removeClient(clientSocket);
    closesocket(clientSocket);
}

bool Server::start(){

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
            continue;
        }
        
        stats.connectedClients++;

        std::thread clientThread(
            &Server::handleClient,
            this,
            clientSocket
        );
        clientThread.detach();
    }
    return true;
}

void Server::sendAOF(SOCKET clientSocket){
    std::ifstream file(Config::appendFile);

    if(!file){
        Logger::error("Unable to open AOF file");
        return;
    }

    std::string data(
        (std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>()
    );

    send(
        clientSocket,
        data.data(),
        static_cast<int>(data.size()),
        0
    );
}

void Server::broadcast(const std::string& command){
    std::lock_guard<std::mutex> lock(replicaMutex);

    for (auto it = replicaSockets.begin(); it != replicaSockets.end(); ) {

        int bytesSent = send(
            *it,
            command.data(),
            static_cast<int>(command.size()),
            0
        );

        if (bytesSent == SOCKET_ERROR) {
            it = replicaSockets.erase(it);
        } else {
            ++it;
        }
    }

    
}

bool Server::isWriteCommand(const Command& cmd) {
    return cmd.command == "SET" ||
        cmd.command == "DEL" ||
        cmd.command == "MSET" ||
        cmd.command == "INCR" ||
        cmd.command == "DECR" ||
        cmd.command == "EXPIRE" ||
        cmd.command == "PERSIST" ||
        cmd.command == "FLUSHDB";
}