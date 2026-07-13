# include <iostream>

void testSetAndGet();
void testMissingKey();
void testRemove();
void testExists();
void testFlushdb();

void testSetParsing();
void testGetParsing();
void testInvalidCommand();

void testSetCommand();
void testGetCommand();
void testPingCommand();
void testGetInvalidArguments();

void testSimpleString();
void testBulkString();
void testInteger();
void testError();

int main()
{
    std::cout << "========== DATABASE TESTS ==========\n";

    testSetAndGet();
    testMissingKey();
    testRemove();
    testExists();
    testFlushdb();

    std::cout << "\n========== PARSER TESTS ==========\n";

    testSetParsing();
    testGetParsing();
    testInvalidCommand();

    std::cout << "\n========== HANDLER TESTS ==========\n";

    testSetCommand();
    testGetCommand();
    testPingCommand();
    testGetInvalidArguments();

    std::cout << "\n========== FORMATTER TESTS ==========\n";

    testSimpleString();
    testBulkString();
    testInteger();
    testError();

    return 0;
}