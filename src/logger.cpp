# include <iostream>
# include "logger.hpp"

void Logger::info(const std::string msg){
    std::cout <<"[INFO] "<<msg<<'\n';
}

void Logger::error(const std::string msg){
    std::cerr <<"[ERROR] "<<msg<<'\n';
}