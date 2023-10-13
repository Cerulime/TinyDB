#pragma once
#include "Runtime.hpp"

class REPL
{
public:
    REPL();
    static void welcome();
    void start();
    const std::string get_now_input() const;
    bool parse_meta_cmd(const std::string &);
    static Runtime::Statement parse_statement(const std::string &);

private:
    inline static const std::string TAB = "    ";
    inline static const std::string BANNER = R"(
 _____  _               ______ ______ 
|_   _|(_)              |  _  \| ___ \
  | |   _  _ __   _   _ | | | || |_/ /
  | |  | || '_ \ | | | || | | || ___ \
  | |  | || | | || |_| || |/ / | |_/ /
  \_/  |_||_| |_| \__, ||___/  \____/ 
                   __/ |              
                  |___/               
)";
    std::string last_input;
    std::string now_input;
    static void manual();
    static void prompt();
    static Runtime::Statement parse_table(Runtime::Statement, const std::string_view &, const std::size_t &, std::size_t &);
    static Runtime::Statement parse_where(Runtime::Statement, const std::string_view &, const std::size_t &, std::size_t &);
    static Runtime::Statement parse_datas(Runtime::Statement, const std::string_view &, const std::size_t &, std::size_t &, const char &);
    static const Runtime::Statement error_statement(const std::string_view &, const std::string &);
};