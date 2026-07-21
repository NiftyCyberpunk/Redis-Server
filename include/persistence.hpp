# pragma once

# include "database.hpp"

class Persistence {
public:
    static bool saveToFile(const Database& db);

    static bool loadFromFile(Database& db);
};