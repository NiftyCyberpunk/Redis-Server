# include <WinSock2.h>
# include <Ws2tcpip.h>
# include <cstddef>
# include <iostream>
# include <chrono>
# include <string>

std::string receiveRaw(SOCKET clientScoket){
    char buffer[1024];

    int byteRecived = recv(
        clientScoket,
        buffer,
        sizeof(buffer) - 1,
        0
    );

    if(byteRecived > 0){
        buffer[byteRecived] = '\0';

        return std::string(buffer, byteRecived);
    }
    return "failed";
}

int main(){
    WSAData wsadata;

    WSAStartup(MAKEWORD(2, 2), &wsadata);

    auto client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(6379);

    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    int result = connect(client, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr));

    if(result == SOCKET_ERROR){
        std::cout <<"Connect failed: "<<WSAGetLastError()<<"\n";
        closesocket(client);
        WSACleanup();
        return -1;
    }

    std::string auth = "*2\r\n$4\r\nAUTH\r\n$8\r\naryan123\r\n";

    send(
        client,
        auth.data(),
        static_cast<int>(auth.size()),
        0
    );

    std::cout <<"Connected and Authenticated"<<"\n";
    
    int k = 0;
    const int batchSize = 10;
    const int totalCommands = 100000;

    auto start = std::chrono::steady_clock::now();
    for(int i = 0; i < totalCommands/batchSize; i++){

        for(int s = 0; s < batchSize; s++){

            std::string key = "key" + std::to_string(k);
            std::string value = "value" + std::to_string(k);

            std::string keylen = std::to_string(key.size());
            std::string valuelen = std::to_string(value.size());

            std::string command = "*3\r\n$3\r\nSET\r\n$" + keylen + "\r\n" + key + "\r\n$" + valuelen + "\r\n" + value + "\r\n";

            int byteSend = send(
                client,
                command.data(),
                static_cast<int>(command.size()),
                0
            );

            if(byteSend == SOCKET_ERROR){
                std::cout <<"Send failed: "<<WSAGetLastError()<<"\n";
            }
            k++;
        }

        int recieved = 0;
        std::string pendingRes;

        while(recieved < batchSize){
            pendingRes += receiveRaw(client);

            while(pendingRes.find("+OK\r\n") != std::string::npos){
                recieved++;
                
                size_t pos = pendingRes.find("+OK\r\n");

                pendingRes.erase(pos, 5);
            }
        }
    }
    auto end = std::chrono::steady_clock::now();

    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    double ops = 100000.0/(elapsed.count()/1000.0);
    double avgLatency = static_cast<double>(elapsed.count() * 1000.0)/totalCommands;

    std::cout <<"========== Redis Benchmark ==========\n\n";

    std::cout <<"Commands       : SET\n";
    std::cout <<"Operations     : "<<totalCommands<<"\n";
    std::cout <<"Batch Size     : "<<batchSize<<"\n";
    std::cout <<"Ops/sec        : "<<ops<<"\n";
    std::cout <<"Average Latency: "<<avgLatency<<"µs\n\n";

    std::cout <<"====================================\n";
}