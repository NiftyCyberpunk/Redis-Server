# ifndef DATABASE_HPP
# define DATABASE_HPP

#include <cstddef>
# include <optional>
# include <unordered_map>
# include <string>
# include <mutex>
#include <vector>

class Database{
private:
    std::unordered_map<std::string, std::string> memory;
    mutable std::mutex memoryMutex; //lock the database so only one thread can access at the moment
    //mutable is used bcz when the mutex is lcoked it changes it state which will not be possible 
    //if the function is const so we tell c++ that this is mutable
    
public:
    void set(const std::string& key, const std::string& value);

    std::optional<std::string> get(const std::string& key) const;

    bool remove(const std::string& key);
    
    bool exists(const std::string& key) const;
    //writing const after fucntion means this function promises not to modify

    std::vector<std::string> getKeys() const;

    void clear();

    std::size_t size() const;

    bool renameKey(const std::string& oldKey, const std::string& newKey);
};

#endif 