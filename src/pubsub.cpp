# include "pubsub.hpp"
# include "protocol_formatter.hpp"
# include <WinSock2.h>
# include <mutex>
# include <unordered_set>
# include <winbase.h>

void PubSub::subscribe(const std::string& channel, SOCKET client){

    std::lock_guard<std::mutex> lock(pubsubMutex);

    channels[channel].insert(client);
}

void PubSub::unsubscribe(const std::string& channel, SOCKET client){

    std::lock_guard<std::mutex> lock(pubsubMutex);

    auto it = channels.find(channel);

    if(it == channels.end()){
        return;
    }

    auto& subscribers = it->second;

    subscribers.erase(client);

    if(subscribers.empty()){
        channels.erase(channel);
    }
}

void PubSub::removeClient(SOCKET client){

    std::lock_guard<std::mutex> lock(pubsubMutex);

    auto it = channels.begin();

    while(it != channels.end()){
        auto& subs = it->second;

        subs.erase(client);

        if(subs.empty()){
            it = channels.erase(it);
        }
        else{
            it++;
        }
    }
}

int PubSub::publish(const std::string& channel, const std::string& message){

    std::unordered_set<SOCKET> subscribers;

    {
        std::lock_guard<std::mutex> lock(pubsubMutex);

        auto it = channels.find(channel);

        if(it == channels.end()){
            return 0;
        }

        subscribers = it->second;
    }
    int count = 0;

    std::string response = ProtocolFormatter::formatPubSub(channel, message);

    for(const auto& client : subscribers){
        int byteSend = send(
            client,
            response.data(),
            static_cast<int>(response.size()),
            0
        );
        if(byteSend != SOCKET_ERROR){
            count++;
        }
    }
    return count;
}