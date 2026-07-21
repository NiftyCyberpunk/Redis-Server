# include "database.hpp"
# include <chrono>
# include <cstddef>
# include <mutex>
# include <optional>
# include <string>
# include <vector>
void Database::set(const std::string& key, const std::string& value){

    std::lock_guard<std::mutex> lock(dbMutex);
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
    expirations.erase(key);
}

std::optional<std::string> Database::get(const std::string& key) const{

    std::lock_guard<std::mutex> lock(dbMutex);

    auto it = memory.find(key);

    if(it != memory.end()){
        return it->second;
    }
    return std::nullopt;    
}

bool Database::remove(const std::string& key){
    
    std::lock_guard<std::mutex> lock(dbMutex);
    expirations.erase(key);
    return memory.erase(key);
}

bool Database::exists(const std::string& key) const{

    std::lock_guard<std::mutex> lock(dbMutex);

    auto it = memory.find(key);

    if(it != memory.end()){
        return true;
    }
    return false;
}

std::vector<std::string> Database::getKeys() const{

    std::lock_guard<std::mutex> lock(dbMutex);

    std::vector<std::string> keys;

    for(const auto& [key, value]:memory){
        keys.push_back(key);
    }
    return keys;
}

void Database::clear(){

    std::lock_guard<std::mutex> lock(dbMutex);

    memory.clear();
    expirations.clear();
}

std::size_t Database::size() const{

    std::lock_guard<std::mutex> lock(dbMutex);
    
    size_t count = memory.size();
    
    return count;
}

bool Database::renameKey(const std::string& oldKey, const std::string& newKey){
    memory[newKey] = memory[oldKey];
    expirations[newKey] = expirations[oldKey];
    expirations.erase(oldKey);
    return memory.erase(oldKey);
}

const std::unordered_map<std::string, std::string>& Database::getAll() const{

    std::lock_guard<std::mutex> lock(dbMutex);
    
    return memory;
}

bool Database::expire(const std::string& key, int seconds){

    std::lock_guard<std::mutex> lock(dbMutex);

    if(memory.find(key) == memory.end()){
        return false;
    }

    expirations[key] = std::chrono::system_clock::now() + std::chrono::seconds(seconds);

    auto it = expirations.find(key);

    return it != expirations.end();
}

int Database::ttl(const std::string& key){

    std::lock_guard<std::mutex> lock(dbMutex);

    auto it = memory.find(key);
    auto itr = expirations.find(key);

    if(it == memory.end()){
        return -2;//Key does not exist
    }
    else if(itr == expirations.end()){
        return -1;//Key present but has no expiration
    }
    
    auto now = std::chrono::system_clock::now();
    auto expiry = itr->second;

    auto remaining = expiry - now;

    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(remaining);

    int ttl = static_cast<int>(seconds.count());

    return ttl;
}

bool Database::persist(const std::string& key){

    std::lock_guard<std::mutex> lock(dbMutex);

    auto it = memory.find(key);
    auto itr = expirations.find(key);

    if(it == memory.end())return false;

    if(itr == expirations.end())return true;

    return expirations.erase(key) > 0;
}

void Database::removeExpiredKey(){

    std::lock_guard<std::mutex> lock(dbMutex);

    const auto& now = std::chrono::system_clock::now();
    auto it = memory.begin();

    while(it != memory.end()){

        auto itr = expirations.find(it->first);
        if(itr != expirations.end() && itr->second <= now){

            expirations.erase(itr);
            it = memory.erase(it);
        }
        else{
            ++it;
        }
    }
}

std::optional<std::chrono::system_clock::time_point> Database::getExpiration(const std::string& key) const{

    std::lock_guard<std::mutex> lock(dbMutex);

    auto itr = expirations.find(key);

    if(itr != expirations.end()){
        return itr->second;
    }
    return std::nullopt;
}

void Database::setExpiration(const std::string& key, std::chrono::system_clock::time_point& expiry){

    std::lock_guard<std::mutex> lock(dbMutex);

    if(memory.find(key) == memory.end()){
        return;
    }

    expirations[key] = expiry;
}