# include "database.hpp"
# include <optional>
void Database::set(const std::string& key, const std::string& value){

    memory[key] = value;
}

std::optional<std::string> Database::get(const std::string& key){

    auto it = memory.find(key);

    if(it != memory.end()){
        return it->second;
    }
    return std::nullopt;    
}

bool Database::remove(const std::string& key){
    
    return memory.erase(key) > 0;
}

bool Database::exists(const std::string& key){
    auto it = memory.find(key);

    if(it != memory.end()){
        return true;
    }
    return false;
}
