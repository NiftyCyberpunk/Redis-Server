# include "server.hpp"
#include <WinSock2.h>
# include <iostream>
#include <minwindef.h>
#include <winSock2.h>

Server::Server():serverSocket(INVALID_SOCKET){

}

bool Server::start(){
    WSAData wsadata;

    int result = WSAStartup(MAKEWORD(2, 2), &wsadata);

    if(result != 0){
        std::cerr << "WSAStartup failed\n";
        return false;
    }
    return true;

    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    /*
        AF_NET = IPv4
        SOCK_STREAM = stream socket
        IPPROTO_TCP = protocol
    */

    if(serverSocket == INVALID_SOCKET){
        std::cerr <<"Socket creation failed\n";
        WSACleanup();
        return false;
    }
}