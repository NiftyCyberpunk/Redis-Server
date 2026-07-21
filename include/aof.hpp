# pragma once

# include "command_handler.hpp"
# include <fstream>
# include <string>

class AOF{
private:
    static bool isReplaying;
    static std::ofstream file;
public:
    static bool init();
    static void close();


    static bool append(const std::string& command);
    static bool replay(CommandHandler& handler);
};