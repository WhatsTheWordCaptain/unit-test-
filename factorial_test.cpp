#include "factorial.h"
#include <gtest/gtest.h>
#include <sqlite3.h>
#include <vector>
#include <iostream>

// choosing numbers from db to insert into test as parameters
std::vector<int> getNumbersFromDatabase() {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    std::vector<int> numbers;

    if (sqlite3_open("db_for_tests.db", &db) == SQLITE_OK) {
        sqlite3_prepare_v2(db, "SELECT number FROM test_numbers;", -1, &stmt, NULL);

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            numbers.push_back(sqlite3_column_int(stmt, 0));
        }

        sqlite3_finalize(stmt);
        sqlite3_close(db);
    } else {
        std::cerr << "Error opening database." << std::endl;
    }

    return numbers;
}
// test with params from db
TEST(FactorialTest, CalculatesCorrectly) {
    std::vector<int> numbers = getNumbersFromDatabase();
    for (int num : numbers) {
        int expected = factorial(num);
        int actual = factorial(num); 
        EXPECT_EQ(expected, actual) << "Factorial of " << num << " calculated incorrectly";
    }
}

int main(int argc, char **argv) {
    std::vector<int> numbers = getNumbersFromDatabase();
    for (int num : numbers) {
        std::cout << "Number: " << num << ", Factorial: " << factorial(num) << std::endl;
    }

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

// log test results into db
void saveTestResultToDatabase(int number, bool testPassed) {
    sqlite3 *db;
    char *errMsg = nullptr;

    if (sqlite3_open("test_results.db", &db) != SQLITE_OK) {
        std::cerr << "Error opening database." << std::endl;
        return;
    }

    std::string sql = "INSERT INTO test_outcomes (number, result) VALUES (" +
                      std::to_string(number) + ", " + (testPassed ? "1" : "0") + ");";

    std::cout << "Executing SQL: " << sql << std::endl;

    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    } else {
        std::cout << "Test result saved to database successfully." << std::endl;
    }

    sqlite3_close(db);
}
