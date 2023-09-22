#include "..\.\include\Runtime.hpp"

bool Runtime::valid_statement(const Runtime::Statement &statement)
{
    return statement.opt != Runtime::Operation::ERRORP;
}