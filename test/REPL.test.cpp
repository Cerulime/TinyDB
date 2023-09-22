#include <iostream>
#include <cassert>
#include "../include/REPL.hpp"

int main() {
    // Test INSERT statement
    std::string input = "INSERT INTO students VALUES ('John', 'Doe', 20, 'Computer Science');";
    Runtime::Statement statement = REPL::parse_statement(input);
    assert(statement.opt == Runtime::Operation::INSERT);
    assert(statement.table == "students");
    assert(statement.datas.size() == 4);
    assert(statement.datas[0] == "John");
    assert(statement.datas[1] == "Doe");
    assert(statement.datas[2] == "20");
    assert(statement.datas[3] == "'Computer Science'");

    // Test DELETE statement
    input = "DELETE FROM students WHERE name LIKE 'J';";
    statement = REPL::parse_statement(input);
    assert(statement.opt == Runtime::Operation::DELETE);
    assert(statement.table == "students");
    assert(statement.datas.size() == 2);
    assert(statement.datas[0] == "name");
    assert(statement.datas[1] == "J");

    // Test UPDATE statement
    input = "UPDATE students SET major = 'Math' WHERE name = 'John';";
    statement = REPL::parse_statement(input);
    assert(statement.opt == Runtime::Operation::UPDATE);
    assert(statement.table == "students");
    assert(statement.datas.size() == 4);
    assert(statement.datas[0] == "major");
    assert(statement.datas[1] == "Math");
    assert(statement.datas[2] == "name");
    assert(statement.datas[3] == "John");

    // Test SELECT statement
    input = "SELECT name, age FROM students WHERE major LIKE 'Science';";
    statement = REPL::parse_statement(input);
    assert(statement.opt == Runtime::Operation::SELECT);
    assert(statement.table == "students");
    assert(statement.datas.size() == 2);
    assert(statement.datas[0] == "name");
    assert(statement.datas[1] == "age");
    assert(statement.datas[2] == "major");
    assert(statement.datas[4] == "Science");

    std::cout << "All tests passed!\n";
    return 0;
}