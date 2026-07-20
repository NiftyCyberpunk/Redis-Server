# include "persistance.hpp"
# include "config.hpp"
# include "database.hpp"
# include <cstddef>
# include <fstream>
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
    for(size_t i = 0; i < count; i++){

        std::getline(file, key);
        std::getline(file, value);

        db.set(key, value);
    }
    return true;
}