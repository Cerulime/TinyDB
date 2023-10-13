#include ".\include\REPL.hpp"

int main()
{
    REPL::welcome();
    while(true)
    {
        REPL::start();
        if (REPL::now_input[0] == '.')
        {
            if (!REPL::parse_meta_cmd(REPL::now_input))
                break;
        }
        else
        {
            Runtime::Statement statement = REPL::parse_statement(REPL::now_input);
            if (!Runtime::valid_statement(statement))
                continue;
            std::vector<std::string> result = Runtime::run_statement(statement);
            std::thread worker(Runtime::scheduler);
            worker.detach();
        }
    }
    if (!Runtime::is_finish())
        Runtime::scheduler();
    std::cout << "Goodbye TinyDB!" << std::endl;
    return 0;
}