#pragma once
#include "IndexTree.hpp"

namespace Runtime
{
    enum class Operation
    {
        ERRORP,
        CREATE,
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
    std::shared_ptr<IndexTree::Tree> tree;
    
    bool valid_statement(const Statement &);
    std::vector<std::string> run_statement(const Statement &);
}