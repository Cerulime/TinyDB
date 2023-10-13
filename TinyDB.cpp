#include ".\include\REPL.hpp"

int main()
{
    REPL repl;
    repl.welcome();
    Runtime runtime;
    while (true)
    {
        repl.start();
        std::string now_input = repl.get_now_input();
        if (now_input[0] == '.')
        {
            if (!repl.parse_meta_cmd(now_input))
                break;
        }
        else
        {
            Runtime::Statement statement = REPL::parse_statement(now_input);
            if (!Runtime::valid_statement(statement))
                continue;
            std::vector<std::string> result = runtime.run_statement(statement);
            std::thread worker(&Runtime::scheduler, &runtime);
            worker.detach();
        }
    }
    if (!runtime.is_finish())
        runtime.scheduler();
    std::cout << "Goodbye TinyDB!" << std::endl;
    return 0;
}