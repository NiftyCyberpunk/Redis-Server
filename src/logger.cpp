# include <iostream>
#include <mutex>
# include "logger.hpp"

std::mutex Logger::logMutex;

void Logger::info(const std::string msg){
    std::lock_guard<std::mutex> lock(logMutex);

    std::cout <<"[INFO] "<<msg<<'\n';
}

void Logger::error(const std::string msg){
    std::lock_guard<std::mutex> lock(logMutex);
    
    std::cerr <<"[ERROR] "<<msg<<'\n';
}

void Logger::replicaInfo(const std::string msg, std::string name){
    std::lock_guard<std::mutex> lock(logMutex);

    std::cout <<"[Replica] "<<name<<" --> "<<msg;
}