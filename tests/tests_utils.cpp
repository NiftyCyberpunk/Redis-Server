# include "tests_utils.hpp"
# include <iostream>


void Utils::msgPrint(bool condition, const std::string& name){
    if(condition){
        std::cout <<"[PASS] "<<name<<'\n';
    }
    else{
        std::cout <<"[FAIL] "<<name<<'\n';
    }
}