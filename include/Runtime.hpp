#pragma once
#include "Include.hpp"

namespace Runtime
{
    enum class Operation
    {
        ERRORP,
        INSERT,
        DELETE,
        UPDATE,
        SELECT
    };
    struct Statement
    {
        Operation opt;
        std::string table;
        std::vector<std::string> datas;
    };
    inline const Statement null_statement = {Operation::ERRORP, "", {}};
    
    bool valid_statement(const Statement &);
}