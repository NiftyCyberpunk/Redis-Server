# include "replica_client.hpp"
# include "command.hpp"
# include "config.hpp"
# include "logger.hpp"
# include "resp_parser.hpp"
# include <WinSock2.h>
# include <Ws2tcpip.h>
# include <iostream>
# include <minwindef.h>
# include <string>


ReplicaClient::ReplicaClient(CommandHandler& commandHandler):masterSocket(INVALID_SOCKET), handler(commandHandler){

}

std::string ReplicaClient::receiveRaw(){
    char buffer[1024];
    int byteReceived = recv(
        masterSocket,
        buffer,
        sizeof(buffer) - 1,
        0
    );

    if(byteReceived > 0){
        buffer[byteReceived] = '\0';

        return std::string(buffer, byteReceived);
    }
    return "Receive failed";
}

bool ReplicaClient::connectToMaster(){

    masterSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    sockaddr_in serverAddr{};

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(Config::masterPort);

    inet_pton(AF_INET, Config::masterHost.c_str(), &serverAddr.sin_addr);

    int result = connect(masterSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr));

    if(result == SOCKET_ERROR){
        std::cout <<"Replica Master connection failed: "<<WSAGetLastError()<<"\n";
        closesocket(masterSocket);
        return false;
    }
    Logger::info("Master connected");
    return true;
}

bool ReplicaClient::start(){

    if(!connectToMaster()){
        return false;
    }

    std::string auth = "*2\r\n$4\r\nAUTH\r\n$8\r\naryan123\r\n";
    Logger::replicaInfo(sendAndReceive(auth), "AUTH");

    std::string command = "*1\r\n$4\r\nPING\r\n";
    Logger::replicaInfo(sendAndReceive(command), "PING");

    command = "*3\r\n$8\r\nREPLCONF\r\n$14\r\nlistening-port\r\n$4\r\n6380\r\n";
    Logger::replicaInfo(sendAndReceive(command), "REPLCONF(listening-port)");

    command = "*3\r\n$8\r\nREPLCONF\r\n$4\r\ncapa\r\n$6\r\npsync2\r\n";
    Logger::replicaInfo(sendAndReceive(command), "REPLCONF(capa)");

    command = "*3\r\n$5\r\nPSYNC\r\n$1\r\n?\r\n$2\r\n-1\r\n";
    Logger::replicaInfo(sendAndReceive(command), "PSYNC");

    //Live Replication
    std::string pending;

    while(true){
        std::string chunk = receiveRaw();

        if(chunk.empty()){
            break;
        }

        pending += chunk;

        while(true){
            Command cmd = RespParser::parse(pending);

            if(!cmd.valid){
                break;
            }
            handler.execute(cmd);

            pending.erase(0, cmd.bytesConsumed);
        }
    }

    return true;
}
std::string ReplicaClient::sendAndReceive(const std::string& command){
    send(
        masterSocket,
        command.data(),
        static_cast<int>(command.size()),
        0
    );
    return receiveRaw();
}