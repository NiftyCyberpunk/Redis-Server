# include "config.hpp"
# include <fstream>
# include <string>

int Config::port = 6379;
std::string Config::dbFile = "data/dump.rdb";
int Config::cleanupInterval = 1;
std::string Config::password = "";

bool  Config::load(const std::string &filename){
    std::ifstream file(filename);

    if(!file){
        return false;
    }
    
    std::string line;

    while(std::getline(file, line)){

        auto pos = line.find('=');

        if(pos == std::string::npos){
            continue;
        }

        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        if(key == "port"){
            port = std::stoi(value);
        }
        if(key == "db_file"){
            dbFile = value;
        }
        if(key == "cleanup_interval"){
            cleanupInterval = std::stoi(value);
        }
        if(key == "require_pass"){
            password = value;
        }
    }
    return true;
}

