# pragma once

# include <WinSock2.h>
# include <mutex>
# include <string>
# include <unordered_map>
# include <unordered_set>

class PubSub{
private:
    std::unordered_map<std::string, std::unordered_set<SOCKET>> channels;
    std::mutex pubsubMutex;
public:
    void subscribe(const std::string& channel, SOCKET client);

    void unsubscribe(const std::string& channel, SOCKET client);

    void removeClient(SOCKET client);

    int publish(const std::string& channel, const std::string& message);
};