# pragma once

#include "database.hpp"
class Persistance {
public:
    static bool saveToFile(const Database& db);

    static bool loadFromFile(Database& db);
};