# pragma once

# include <mutex>
# include <string>
class Logger {
private:
    static std::mutex logMutex;
public:
    static void info(const std::string msg);
    static void replicaInfo(const std::string msg, std::string name);
    static void error(const std::string msg);
};