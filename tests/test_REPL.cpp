#include "..\include\REPL.hpp"
#include <gtest/gtest.h>

TEST(REPLTest, Create)
{
    REPL repl;
    std::string input = "CREATE student (id,name,age);";
    Runtime::Statement statement = repl.parse_statement(input);
    ASSERT_TRUE(Runtime::valid_statement(statement));
    EXPECT_EQ(statement.opt, Runtime::Operation::CREATE);
    EXPECT_EQ(statement.table, "student");
    EXPECT_EQ(statement.datas.size(), 3);
    EXPECT_EQ(statement.datas[0], "id");
    EXPECT_EQ(statement.datas[1], "name");
    EXPECT_EQ(statement.datas[2], "age");
}

TEST(REPLTest, Insert)
{
    REPL repl;
    std::string input = "INSERT INTO student VALUES (1,\"Tom\",18);";
    Runtime::Statement statement = repl.parse_statement(input);
    ASSERT_TRUE(Runtime::valid_statement(statement));
    EXPECT_EQ(statement.opt, Runtime::Operation::INSERT);
    EXPECT_EQ(statement.table, "student");
    EXPECT_EQ(statement.datas.size(), 3);
    EXPECT_EQ(statement.datas[0], "1");
    EXPECT_EQ(statement.datas[1], "Tom");
    EXPECT_EQ(statement.datas[2], "18");
}

TEST(REPLTest, Delete)
{
    REPL repl;
    std::string input = "DELETE FROM student WHERE id LIKE \"1\";";
    Runtime::Statement statement = repl.parse_statement(input);
    ASSERT_TRUE(Runtime::valid_statement(statement));
    EXPECT_EQ(statement.opt, Runtime::Operation::DELETE);
    EXPECT_EQ(statement.table, "student");
    EXPECT_EQ(statement.datas.size(), 2);
    EXPECT_EQ(statement.datas[0], "id");
    EXPECT_EQ(statement.datas[1], "1");
}

TEST(REPLTest, Update)
{
    REPL repl;
    std::string input = "UPDATE student SET name=\"Tom\" WHERE id LIKE \"1\";";
    Runtime::Statement statement = repl.parse_statement(input);
    ASSERT_TRUE(Runtime::valid_statement(statement));
    EXPECT_EQ(statement.opt, Runtime::Operation::UPDATE);
    EXPECT_EQ(statement.table, "student");
    EXPECT_EQ(statement.datas.size(), 4);
    EXPECT_EQ(statement.datas[0], "name");
    EXPECT_EQ(statement.datas[1], "Tom");
    EXPECT_EQ(statement.datas[2], "id");
    EXPECT_EQ(statement.datas[3], "1");
}

TEST(REPLTest, Select)
{
    REPL repl;
    std::string input = "SELECT * FROM student WHERE id LIKE \"1\";";
    Runtime::Statement statement = repl.parse_statement(input);
    ASSERT_TRUE(Runtime::valid_statement(statement));
    EXPECT_EQ(statement.opt, Runtime::Operation::SELECT);
    EXPECT_EQ(statement.table, "student");
    EXPECT_EQ(statement.datas.size(), 3);
    EXPECT_EQ(statement.datas[0], "*");
    EXPECT_EQ(statement.datas[1], "id");
    EXPECT_EQ(statement.datas[2], "1");
}