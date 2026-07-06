# include <iostream>
//# include <optional>
# include "database.hpp"
# include <string>


int main(){

    std::cout <<"C++ Redis Server\n";

    Database db;
    db.set("name", "Aryan Verma");

    std::cout <<"Value stored successfully\n";

    auto name = db.get("age");
    if(!name.has_value()){
        std::cout <<"Key not found"<< std::endl;
    }
    else{
        std::cout <<"Name: "<<name.value()<< std::endl;
    }

    std::cout <<db.exists("name")<< std::endl;
    std::cout <<db.remove("name")<< std::endl;
    std::cout <<db.exists("name")<< std::endl;

    return 0;
}