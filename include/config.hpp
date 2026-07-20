#pragma once

#include <string>

class Config {
public:
    static bool load(const std::string& filename);

    static int port;
    static std::string dbFile;
    static int cleanupInterval;
    static std::string password;
};