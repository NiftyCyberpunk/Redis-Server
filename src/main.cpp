# include <iostream>
# include "database.hpp"


int main(){

    std::cout <<"C++ Redis Server\n";

    Database db;
    db.set("name", "Aryan Verma");

    std::cout <<"Value stored successfully\n";

    db.get("name");
    std::cout <<db.exists("name")<<std::endl;
    std::cout <<db.remove("name")<<std::endl;
    std::cout <<db.exists("name")<<std::endl;

    return 0;
}