# ifndef DATABASE_HPP
# define DATABASE_HPP

# include <optional>
# include <unordered_map>
# include <string>
# include <mutex>

class Database{
private:
    std::unordered_map<std::string, std::string> memory;
    mutable std::mutex memoryMutex; //lock the database so only one thread can access at the moment
public:
    void set(const std::string& key, const std::string& value);

    std::optional<std::string> get(const std::string& key) const;

    bool remove(const std::string& key);
    
    bool exists(const std::string& key) const;
    //writing const after fucntion means this function promises not to modify
};

#endif 