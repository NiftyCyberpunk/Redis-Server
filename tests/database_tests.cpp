# include "database.hpp"
# include "tests_utils.hpp"

void testSetAndGet(){
    Database db;

    db.set("name","Aryan");

    auto value = db.get("name");

   Utils::msgPrint((value && *value == "Aryan"), "SetAndGet");
}
void testMissingKey(){
    Database db;

    auto value = db.get("age");

    Utils::msgPrint(!value, "MissingKey");
}
void testRemove(){
    Database db;

    db.set("name","Aryan");

    db.remove("name");

    Utils::msgPrint((!db.get("name")), "Remove");
}
void testExists(){
    Database db;

    db.set("name","Aryan");
    Utils::msgPrint((db.exists("name")), "Exists");

}

void testFlushdb(){
    Database db;

    db.set("name", "Aryan");
    db.set("age", "18");

    db.clear();

    Utils::msgPrint(!db.exists("name"), "Flushdb");
    Utils::msgPrint(!db.exists("age"), "Flushdb");
}