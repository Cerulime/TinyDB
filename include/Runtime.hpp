#pragma once
#include "IndexTree.hpp"
#include "FileOperation.hpp"

class Runtime
{
public:
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
        Statement(Operation opt = Operation::ERRORP, std::string table = "", std::vector<std::string> datas = {}) : opt(opt), table(table), datas(datas) {}
    };
    static bool valid_statement(const Statement &);
    std::vector<std::string> run_statement(const Statement &);
    void scheduler();
    bool is_finish();

private:
    IndexTree cache;
    FileOperation fileOp;
};