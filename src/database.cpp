# include "database.hpp"
# include <mutex>
# include <optional>
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
