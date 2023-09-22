#include ".\include\REPL.hpp"
#include ".\include\HashTable.hpp"

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
            
        }
    }
    std::cout << "Goodbye TinyDB!" << std::endl;
    return 0;
}