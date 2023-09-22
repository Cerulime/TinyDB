#include "Runtime.hpp"

namespace REPL {
    const std::string TAB = "    ";
    const std::string BANNER = R"(
 _____  _               ______ ______ 
|_   _|(_)              |  _  \| ___ \
  | |   _  _ __   _   _ | | | || |_/ /
  | |  | || '_ \ | | | || | | || ___ \
  | |  | || | | || |_| || |/ / | |_/ /
  \_/  |_||_| |_| \__, ||___/  \____/ 
                   __/ |              
                  |___/               
)";

    extern std::string last_input;
    extern std::string now_input;

    void welcome();
    void manual();
    void prompt();
    void start();
    bool parse_meta_cmd(const std::string &);
    Runtime::Statement parse_statement(const std::string &);
    Runtime::Statement parse_table(Runtime::Statement, const std::string_view &,const std::size_t &, std::size_t &);
    Runtime::Statement parse_where(Runtime::Statement, const std::string_view &,const std::size_t &, std::size_t &);
    Runtime::Statement parse_datas(Runtime::Statement, const std::string_view &,const std::size_t &, std::size_t &, const char &);
    Runtime::Statement error_statement(const std::string_view &, const std::string &);
}