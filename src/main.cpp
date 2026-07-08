# include <iostream>
# include "database.hpp"
# include "command_handler.hpp"
# include "server.hpp"
//# include <optional>

int main(){

    Database db;
    CommandHandler handler(db);
    Server server(handler);

    if(!server.start()){
        std::cout <<"Server failed to start\n";
        return -1;
    }
    std::cout <<"Server started\n";

    return 0;
}