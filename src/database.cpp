# include "database.hpp"
#include <cstddef>
# include <mutex>
# include <optional>
#include <string>
#include <vector>
void Database::set(const std::string& key, const std::string& value){

    std::lock_guard<std::mutex> lock(memoryMutex);
    /*
        lock_gaurd
        creates a lock gaurd object named lock 
        it lock the mutex at the contruction and at the end of scope it unlock it automatically
        even if it --> return early
                        throws an error
                        many returns
        it prevents us to manually lock and unlock it
    */
    memory[key] = value;
}

std::optional<std::string> Database::get(const std::string& key) const{

    std::lock_guard<std::mutex> lock(memoryMutex);

    auto it = memory.find(key);

    if(it != memory.end()){
        return it->second;
    }
    return std::nullopt;    
}

bool Database::remove(const std::string& key){
    
    std::lock_guard<std::mutex> lock(memoryMutex);

    return memory.erase(key);
}

bool Database::exists(const std::string& key) const{

    std::lock_guard<std::mutex> lock(memoryMutex);

    auto it = memory.find(key);

    if(it != memory.end()){
        return true;
    }
    return false;
}

std::vector<std::string> Database::getKeys() const{

    std::lock_guard<std::mutex> lock(memoryMutex);

    std::vector<std::string> keys;

    for(const auto& [key, value]:memory){
        keys.push_back(key);
    }
    return keys;
}

void Database::clear(){

    std::lock_guard<std::mutex> lock(memoryMutex);

    memory.clear();
}

std::size_t Database::size() const{

    std::lock_guard<std::mutex> lock(memoryMutex);
    
    size_t count = memory.size();
    
    return count;
}

bool Database::renameKey(const std::string& oldKey, const std::string& newKey){
    memory[newKey] = memory[oldKey];

    return memory.erase(oldKey);
}

    const std::unordered_map<std::string, std::string>& Database::getAll() const{

        std::lock_guard<std::mutex> lock(memoryMutex);
        
        return memory;
    }
