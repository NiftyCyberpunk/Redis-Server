# include <iostream>
# include <string>
# include "database.hpp"
# include "command_parser.hpp"
# include "command_handler.hpp"
//# include <optional>

int main(){

    std::cout <<"C++ Redis Server\n";
    Database db;
    CommandHandler handler(db);
    std::string input;

    while (true){
        std::cout <<"> ";
        
        std::getline(std::cin, input);
        /*
            Why "std::getline" not "std::cin"???
                Because "std::cin" stops reading when it hits space,
                Ex => "Aryan Verma" it only reads "Aryan"
                while "std::getline" reads until enter or newline char is entered... 
        */

        if(input.empty()){
            continue;
        }
        if(input == "EXIT" || input == "exit"){
            std::cout <<"Pleasure working with you...Bye"<<std::endl;

            break;
        }

        Command cmd = CommandParser::parser(input);

        std::string response = handler.execute(cmd);

        std::cout << response << '\n';
    }
    return 0;
}