# include "config.hpp"
#include <cstddef>
# include <fstream>
# include <string>

int Config::port = 6379;
std::string Config::dbFile = "data/dump.rdb";
int Config::cleanupInterval = 1;
std::string Config::password = "";

bool Config::appendOnly = false;
std::string Config::appendFile = "data/master.aof";

int Config::replicaPort = 6380;
bool Config::isReplica = true;
std::string Config::replicaOf = "127.0.0.1:6379";
std::string Config::masterHost = "127.0.0.1";
int Config::masterPort = 6379;

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
        if(key == "append_only"){
            if(value == "yes"){
                appendOnly = true;
            }
            else{
                appendOnly = false;
            }
        }
        if(key == "append_file"){
            appendFile = value;
        }
        if(key == "replica_port"){
            replicaPort = std::stoi(value);
        }
        if(key == "replica_of"){
            replicaOf = value;
            if(!replicaOf.empty()){
                size_t pos = replicaOf.find(':');

                masterHost = replicaOf.substr(0, pos);
                masterPort = std::stoi(replicaOf.substr(pos + 1));

                isReplica = true;
            }
            else{
                isReplica = false;
            }
        }
    }
    return true;
}

