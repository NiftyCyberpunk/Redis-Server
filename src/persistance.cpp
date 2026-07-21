# include "persistance.hpp"
# include "config.hpp"
# include "database.hpp"
#include <chrono>
# include <cstddef>
# include <fstream>
#include <ios>
# include <string>
# include <limits>

bool Persistance::saveToFile(const Database& db){

    std::ofstream file(Config::dbFile);

    if(!file){
        return false;
    }
    const auto& data = db.getAll();

    file <<data.size()<<'\n';

    for(const auto& [key, value] : db.getAll()){
        file <<key<< '\n';
        file <<value<< '\n';

        auto expiry = db.getExpiration(key);

        if(!expiry){
            file <<-1<<'\n';
        }
        else{
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(expiry->time_since_epoch()).count();

            file <<ms<<'\n';
        }
    }

    //file.close(); file closes itself bcz of deconstructure so it optional
    return true;
}

bool Persistance::loadFromFile(Database &db){
    std::ifstream file(Config::dbFile);

    if(!file){
        return false;
    }

    std::size_t count = 0;
    if(!(file >>count)) return true;
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::string key;
    std::string value;
    long long timeStamps;
    for(size_t i = 0; i < count; i++){

        std::getline(file, key);
        std::getline(file, value);
       

        file >>timeStamps;
        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        db.set(key, value);
        
        if(timeStamps == -1){
            continue;
        }

        // timeStamps stored as milliseconds since epoch; construct milliseconds duration
        auto expiry = std::chrono::system_clock::time_point(std::chrono::milliseconds(timeStamps));

        if(expiry <= std::chrono::system_clock::now()){
            db.remove(key);
            continue;
        }
        
        db.setExpiration(key, expiry);
    }
    return true;
}