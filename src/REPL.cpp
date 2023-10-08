#include "..\include\REPL.hpp"

std::string REPL::last_input = "";
std::string REPL::now_input = "None.";

void REPL::welcome()
{
    std::cout << REPL::BANNER << std::endl;
    std::cout << "This is TinyDB. Welcome!" << std::endl << std::endl;

    REPL::manual();
}

void REPL::manual()
{
    std::cout << "Manual for u :)" << std::endl;
    std::cout << "========================" << std::endl;

    std::cout << REPL::TAB << ".help -> Manual" << std::endl;
    std::cout << REPL::TAB << ".last -> Last input" << std::endl;
    std::cout << REPL::TAB << ".exit -> Exit" << std::endl;
}

void REPL::prompt()
{
    std::cout << "TinyDB > ";
}

void REPL::start()
{
    REPL::prompt();
    char input[210];
    std::cin.getline(input, 200);
    REPL::last_input = REPL::now_input;
    REPL::now_input = input;
}

/**
 * @brief Parses the meta command input and executes the corresponding command.
 * 
 * @param input The input string containing the meta command.
 * @return true If the command is executed successfully.
 * @return false If the command is ".exit".
 */
bool REPL::parse_meta_cmd(const std::string &input)
{
    std::unordered_map<std::string, std::function<bool()>> commands = {
        {".help", []() { REPL::manual(); return true; }},
        {".last", []() { std::cout << REPL::last_input << std::endl; return true; }},
        {".exit", []() { std::cout << "Exiting..."; return false; }}
    };

    auto it = commands.find(input);
    if (it != commands.end())
        return it->second();
    else
    {
        std::cout << "Unknown command: " << input << std::endl;
        return true;
    }
}

/**
 * @brief This function is used to handle unknown statements in the REPL.
 * 
 * @param input The input string that contains the unknown statement.
 * @param caller The name of the function that called this error statement function.
 * @return A null statement from the Runtime namespace.
 */
Runtime::Statement REPL::error_statement(const std::string_view &input, const std::string &caller)
{
    std::cout << "Caller function:" << caller << std::endl;
    std::cout << "Unknown statement: " << input << std::endl;
    return Runtime::null_statement;
}

/**
 * Parses the table name from the input string and sets it in the given statement object.
 * 
 * @param statement The statement object to set the table name in.
 * @param input The input string to parse the table name from.
 * @param len The length of the input string.
 * @param p The current position in the input string.
 * @return The statement object with the table name set, or an error statement if the table name is empty.
 */
Runtime::Statement REPL::parse_table(Runtime::Statement statement, const std::string_view &input,const std::size_t &len, std::size_t &p)
{
    for (int i = 0; p + i < len; i++)
    {
        if (input[p + i] != ' ' && input[p + i] != ';')
            continue;
        statement.table = std::string(input.substr(p, i));
        p = p + i;
        break;
    }
    if (statement.table.empty())
        return REPL::error_statement(input, __func__);
    return statement;
}

/**
 * @brief Parses the "WHERE" clause of a SQL statement.
 * 
 * @param statement The statement to be parsed.
 * @param input The input string to be parsed.
 * @param len The length of the input string.
 * @param p The current position in the input string.
 * @return Runtime::Statement The parsed statement.
 */
Runtime::Statement REPL::parse_where(Runtime::Statement statement, const std::string_view &input,const std::size_t &len, std::size_t &p)
{
    auto process_input = [&statement, &input, &p](char delimiter) -> bool {
        auto next = input.find(delimiter, p);
        if (next == std::string::npos)
            return false;
        statement.datas.push_back(input.substr(p, next - p).data());
        p = next;
        return true;
    };

    bool success;
    auto last_size = statement.datas.size();
    success = process_input(' ');
    auto now_size = statement.datas.size();
    if (!success || now_size != last_size + 1 || input.compare(p + 1, 4, "LIKE"))
        return REPL::error_statement(input, __func__);

    p = p + 7;
    last_size = now_size;
    success = process_input('\'');
    now_size = statement.datas.size();
    if (!success || now_size != last_size + 1 || p != len - 2)
        return REPL::error_statement(input, __func__);
    
    return statement;
}

/**
 * Parses the input string to extract data statements separated by commas and semicolons.
 * @param statement The statement to be updated with the extracted data.
 * @param input The input string to be parsed.
 * @param len The length of the input string.
 * @param p The current position in the input string.
 * @param eof The end of file character.
 * @return The updated statement with the extracted data.
 */
Runtime::Statement REPL::parse_datas(Runtime::Statement statement, const std::string_view &input,const std::size_t &len, std::size_t &p, const char &eof)
{
    auto last_size = statement.datas.size();

    while (p < len && input[p] != ';')
    {
        for (int i = 0; p + i < len; i++)
        {
            if (input[p + i] != ',' && input[p + i] != eof)
                continue;
            statement.datas.push_back(input.substr(p, i).data());
            p = p + i;
            break;
        }
        p++;
    }

    auto now_size = statement.datas.size();
    if(now_size == last_size)
        return REPL::error_statement(input, __func__);
    return statement;
}

/**
 * @brief Parses the input string and returns a Runtime::Statement object.
 * 
 * @param input_raw The input string to be parsed.
 * @return Runtime::Statement The parsed statement object.
 */
Runtime::Statement REPL::parse_statement(const std::string &input_raw)
{
    Runtime::Statement statement = {};
    std::string_view input = input_raw;
    auto len = input.length();
    if (!input.compare(0, 6, "CREATE"))
    {
        statement.opt = Runtime::Operation::CREATE;

        std::size_t p = 7;
        statement = REPL::parse_table(statement, input, len, p);
        if (!Runtime::valid_statement(statement))
            return REPL::error_statement(input, __func__);
        
        p = p + 2;
        statement = REPL::parse_datas(statement, input, len, p, ')');
        if (!Runtime::valid_statement(statement) || input[p] != ';')
            return REPL::error_statement(input, __func__);
        
        return statement;
    }
    else if (!input.compare(0, 11, "INSERT INTO"))
    {
        statement.opt = Runtime::Operation::INSERT;

        std::size_t p = 12;
        statement = REPL::parse_table(statement, input, len, p);

        if (!Runtime::valid_statement(statement) || input.compare(p + 1, 6, "VALUES"))
            return REPL::error_statement(input, __func__);
        
        p = p + 9;
        statement = REPL::parse_datas(statement, input, len, p, ')');
        if (!Runtime::valid_statement(statement) || input[p] != ';')
            return REPL::error_statement(input, __func__);

        return statement;
    }
    else if (!input.compare(0, 11, "DELECT FROM"))
    {
        statement.opt = Runtime::Operation::DELETE;

        std::size_t p = 12;
        statement = REPL::parse_table(statement, input, len, p);

        if (!Runtime::valid_statement(statement))
            return REPL::error_statement(input, __func__);
        if (input[p] == ';')
            return statement;
        
        if (input.compare(p + 1, 5, "WHERE"))
            return REPL::error_statement(input, __func__);
        p = p + 7;
        statement = REPL::parse_where(statement, input, len, p);
        return statement;
    }
    else if (!input.compare(0, 6, "UPDATE"))
    {
        statement.opt = Runtime::Operation::UPDATE;

        std::size_t p = 7;
        statement = REPL::parse_table(statement, input, len, p);
        
        if (!Runtime::valid_statement(statement) || input.compare(p + 1, 3, "SET"))
            return REPL::error_statement(input, __func__);

        auto process_input = [&statement, &input, &p](char delimiter) -> bool {
            auto next = input.find(delimiter, p);
            if (next == std::string::npos)
                return false;
            statement.datas.push_back(input.substr(p, next - p).data());
            p = next;
            return true;
        };

        p = p + 5;
        if (!process_input('='))
            return REPL::error_statement(input, __func__);

        p = p + 2;
        if (!process_input('\''))
            return REPL::error_statement(input, __func__);

        p = p + 1;
        statement = REPL::parse_where(statement, input, len, p);
        return statement;
    }
    else if (!input.compare(0, 6, "SELECT"))
    {
        statement.opt = Runtime::Operation::SELECT;

        std::size_t p = 7;
        statement = REPL::parse_datas(statement, input, len, p, ' ');
        if (!Runtime::valid_statement(statement) || input.compare(p + 1, 4, "FROM"))
            return REPL::error_statement(input, __func__);
        
        p = p + 6;
        statement = REPL::parse_table(statement, input, len, p);
        if (!Runtime::valid_statement(statement) || input.compare(p + 1, 5, "WHERE"))
            return REPL::error_statement(input, __func__);
        
        p = p + 7;
        statement = REPL::parse_where(statement, input, len, p);
        return statement;
    }
    else
    {
        return REPL::error_statement(input, __func__);
    }
}