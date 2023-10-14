#include "..\include\REPL.hpp"

REPL::REPL()
{
    this->last_input = "";
    this->now_input = "None.";
}

void REPL::welcome()
{
    std::cout << BANNER << std::endl;
    std::cout << "This is TinyDB. Welcome!" << std::endl
              << std::endl;

    manual();
}

void REPL::manual()
{
    std::cout << "Manual for u :)" << std::endl;
    std::cout << "========================" << std::endl;

    std::cout << TAB << ".help -> Manual" << std::endl;
    std::cout << TAB << ".last -> Last input" << std::endl;
    std::cout << TAB << ".load -> Load a file as input" << std::endl;
    std::cout << TAB << ".exit -> Exit" << std::endl;
}

void REPL::prompt()
{
    std::cout << "TinyDB > ";
}

void REPL::start()
{
    prompt();
    char input[210];
    std::cin.getline(input, 200);
    this->last_input = this->now_input;
    this->now_input = input;
}

const std::string REPL::get_now_input() const
{
    return this->now_input;
}

/**
 * @brief Parses the meta command input and executes the corresponding function.
 *
 * @param input The input string containing the meta command.
 * @return true If the meta command is valid and executed successfully.
 * @return false If the meta command is ".exit".
 */
bool REPL::parse_meta_cmd(const std::string &input)
{
    auto me = this;
    std::unordered_map<std::string, std::function<bool()>> commands = {
        {".help", []()
         { manual(); return true; }},
        {".last", [&me]()
         { std::cout << me->last_input << std::endl; return true; }},
        {".load", []()
         { std::cout << "The file path: "; return true; }},
        {".exit", []()
         { std::cout << "Exiting..." << std::endl; return false; }}};

    auto it = commands.find(input);
    if (it != commands.end())
        return it->second();
    else
    {
        std::cout << "Unknown command: " << input << std::endl;
        return true;
    }
}

const Runtime::Statement REPL::error_statement(const std::string_view &input, const std::string &caller, const int &line)
{
    std::cout << "Caller function: " << caller << std::endl;
    std::cout << "Caller at: " << line << std::endl;
    std::cout << "Unknown statement: " << input << std::endl;
    return Runtime::Statement(Runtime::Operation::ERRORP, "", {});
}

/**
 * Parses the table name from the input string and sets it in the given statement object.
 *
 * @param statement The statement object to set the table name in.
 * @param input The input string to parse the table name from.
 * @param len The length of the input string.
 * @param p The current position in the input string.
 * @return The statement object with the table name set, or an error statement if the table name could not be parsed.
 */
Runtime::Statement REPL::parse_table(Runtime::Statement statement, const std::string_view &input, const std::size_t &len, std::size_t &p)
{
    for (int i = 0; p + i < len; i++)
    {
        if (input[p + i] != ' ')
            continue;
        statement.table = std::string(input.substr(p, i));
        p = p + i;
        break;
    }
    if (statement.table.empty())
        return error_statement(input, __func__, __LINE__);
    return statement;
}

/**
 * Parses the input string to extract the WHERE clause of a SQL statement.
 *
 * @param statement The statement object to store the extracted data.
 * @param input The input string to parse.
 * @param len The length of the input string.
 * @param p The current position in the input string.
 * @return The statement object with the extracted data, or an error statement if parsing fails.
 */
Runtime::Statement REPL::parse_where(Runtime::Statement statement, const std::string_view &input, const std::size_t &len, std::size_t &p)
{
    auto process_input = [&statement, &input, &p](char delimiter) -> bool
    {
        auto next = input.find(delimiter, p);
        if (next == std::string::npos)
            return false;
        statement.datas.push_back(std::string(input.substr(p, next - p)));
        p = next;
        return true;
    };

    bool success;
    auto last_size = statement.datas.size();
    success = process_input(' ');
    auto now_size = statement.datas.size();
    if (!success || now_size != last_size + 1 || input.compare(p + 1, 4, "LIKE"))
        return error_statement(input, __func__, __LINE__);

    p = p + 7;
    last_size = now_size;
    success = process_input('\"');
    now_size = statement.datas.size();
    if (!success || now_size != last_size + 1 || input.at(p + 1) != ';')
        return error_statement(input, __func__, __LINE__);

    return statement;
}

/**
 * Parses the input string and extracts the data statements separated by commas.
 *
 * @param statement The statement object to store the extracted data statements.
 * @param input The input string to be parsed.
 * @param len The length of the input string.
 * @param p The current position in the input string.
 * @param eof The end of file character.
 *
 * @return The statement object with the extracted data statements.
 */
Runtime::Statement REPL::parse_datas(Runtime::Statement statement, const std::string_view &input, const std::size_t &len, std::size_t &p, const char &eof)
{
    auto last_size = statement.datas.size();

    while (p < len && input.at(p) != ';')
    {
        for (int i = 0; p + i < len; i++)
        {
            if (input.at(p + i) != ',' && input.at(p + i) != eof)
                continue;
            statement.datas.push_back(std::string(input.substr(p, i)));
            p = p + i;
            break;
        }
        if (input.at(p) == eof)
            break;
        p++;
    }

    auto now_size = statement.datas.size();
    if (now_size == last_size)
        return error_statement(input, __func__, __LINE__);
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
    if (len == 0 || !input.at(len - 1) == ';')
        return error_statement(input, __func__, __LINE__);
    if (!input.compare(0, 6, "CREATE"))
    {
        statement.opt = Runtime::Operation::CREATE;

        std::size_t p = 7;
        statement = parse_table(statement, input, len, p);

        p = p + 2;
        statement = parse_datas(statement, input, len, p, ')');
        if (input.at(p + 1) != ';')
            return error_statement(input, __func__, __LINE__);

        return statement;
    }
    else if (!input.compare(0, 11, "INSERT INTO"))
    {
        statement.opt = Runtime::Operation::INSERT;

        std::size_t p = 12;
        statement = parse_table(statement, input, len, p);

        if (input.compare(p + 1, 6, "VALUES"))
            return error_statement(input, __func__, __LINE__);

        p = p + 9;
        statement = parse_datas(statement, input, len, p, ')');
        if (input.at(p + 1) != ';')
            return error_statement(input, __func__, __LINE__);

        for (auto &x : statement.datas)
            if (x[0] == '\"')
                x = x.substr(1, x.length() - 2);
        return statement;
    }
    else if (!input.compare(0, 11, "DELETE FROM"))
    {
        statement.opt = Runtime::Operation::DELETE;

        std::size_t p = 12;
        statement = parse_table(statement, input, len, p);
        if (input.compare(p + 1, 5, "WHERE"))
            return error_statement(input, __func__, __LINE__);

        p = p + 7;
        statement = parse_where(statement, input, len, p);
        return statement;
    }
    else if (!input.compare(0, 6, "UPDATE"))
    {
        statement.opt = Runtime::Operation::UPDATE;

        std::size_t p = 7;
        statement = parse_table(statement, input, len, p);

        if (input.compare(p + 1, 3, "SET"))
            return error_statement(input, __func__, __LINE__);

        auto process_input = [&statement, &input, &p](char delimiter) -> bool
        {
            auto next = input.find(delimiter, p);
            if (next == std::string::npos)
                return false;
            statement.datas.push_back(std::string(input.substr(p, next - p)));
            p = next;
            return true;
        };

        p = p + 5;
        if (!process_input('='))
            return error_statement(input, __func__, __LINE__);

        p = p + 2;
        if (!process_input('\"'))
            return error_statement(input, __func__, __LINE__);

        p = p + 1;
        if (input.compare(p + 1, 5, "WHERE"))
            return error_statement(input, __func__, __LINE__);
        p = p + 7;
        statement = parse_where(statement, input, len, p);
        return statement;
    }
    else if (!input.compare(0, 6, "SELECT"))
    {
        statement.opt = Runtime::Operation::SELECT;

        std::size_t p = 7;
        statement = parse_datas(statement, input, len, p, ' ');
        if (input.compare(p + 1, 4, "FROM"))
            return error_statement(input, __func__, __LINE__);

        p = p + 6;
        statement = parse_table(statement, input, len, p);
        if (input.compare(p + 1, 5, "WHERE"))
            return error_statement(input, __func__, __LINE__);

        p = p + 7;
        statement = parse_where(statement, input, len, p);
        return statement;
    }
    else
    {
        return error_statement(input, __func__, __LINE__);
    }
}