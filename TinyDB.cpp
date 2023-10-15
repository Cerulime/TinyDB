#include ".\include\REPL.hpp"

REPL repl;
Runtime runtime;

int main()
{
    repl.welcome();
    while (true)
    {
        repl.start();
        std::string now_input = repl.get_now_input();
        if (now_input.empty())
        {
            std::cout << std::endl;
            continue;
        }

        auto timer_begin = std::chrono::high_resolution_clock::now();

        if (now_input[0] == '.')
        {
            if (!repl.parse_meta_cmd(now_input))
                break;
            if (now_input == ".load")
            {
                std::string filepath;
                std::cin >> filepath;
                std::ifstream infile(filepath);
                std::string line;
                int all = 0, success = 0;
                std::vector<std::string> result;
                while (std::getline(infile, line))
                {
                    if (line.empty())
                        break;
                    all++;
                    Runtime::Statement statement = REPL::parse_statement(line);
                    if (!Runtime::valid_statement(statement))
                        continue;
                    result = runtime.run_statement(statement);
                    success++;
                    if (success % 10000 == 0)
                        std::cout << "Loading... " << success << " of " << all << " lines." << std::endl;
                }
                infile.close();
                std::thread worker(&Runtime::scheduler, &runtime);
                worker.detach();
                std::cout << "-------------" << std::endl;
                std::cout << "Task finished" << std::endl;
                std::cout << "Load " << success << " of " << all << " lines." << std::endl;
                for (auto &x : result)
                    std::cout << x << std::endl;
            }
        }
        else
        {
            Runtime::Statement statement = REPL::parse_statement(now_input);
            if (!Runtime::valid_statement(statement))
                continue;
            std::vector<std::string> result = runtime.run_statement(statement);
            for (auto &x : result)
                std::cout << x << std::endl;
            if (statement.opt != Runtime::Operation::SELECT)
            {
                std::thread worker(&Runtime::scheduler, &runtime);
                worker.detach();
            }
        }
#ifdef DEBUG
        assert(runtime.check());
#endif

        auto timer_end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(timer_end - timer_begin);
        std::cout << "Time measured: " << duration.count() * 1e-3 << " ms." << std::endl;
        std::cout << "-------------" << std::endl;
    }
    if (!runtime.is_finish())
        runtime.scheduler();
    std::cout << "Goodbye TinyDB!" << std::endl;
    return 0;
}