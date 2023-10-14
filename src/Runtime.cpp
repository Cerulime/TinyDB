#include "..\include\Runtime.hpp"

bool Runtime::valid_statement(const Statement &statement)
{
    return statement.opt != Operation::ERRORP;
}

/**
 * Executes a statement and returns the result as a vector of strings.
 * @param statement The statement to execute.
 * @return A vector of strings representing the result of the statement.
 */
std::vector<std::string> Runtime::run_statement(const Statement &statement)
{
    std::vector<std::string> result;
    std::string key;
    switch (statement.opt)
    {
    case Operation::CREATE:
        this->cache.table_name = statement.table;
        this->cache.tree = this->cache.build(statement.datas);
        result = this->cache.show_columns(this->cache.tree);
        break;
    case Operation::INSERT:
        if (this->cache.table_name != statement.table)
            break;
        for (auto &i : statement.datas)
            key += i + ",";
        this->cache.insert(this->cache.tree, key, statement.datas);
        result = this->cache.show_columns(this->cache.tree);
        break;
    case Operation::DELETE:
    {
        if (this->cache.table_name != statement.table)
            break;
        key = statement.datas[1];
        std::vector<std::shared_ptr<IndexTree::LeafNode>> leafs = this->cache.fuzzy_find_leaf(this->cache.tree, key);
        std::vector<std::string> keys;
        int index = 0;
        auto size = this->cache.tree->column.size();
        while (index < size && this->cache.tree->column[index] != statement.datas[0])
            index++;
        for (auto &leaf : leafs)
            for (auto &data : leaf->datas)
                // if (data.first.find(key) != std::string::npos)
                if (data.second[index].find(key) != std::string::npos)
                    keys.push_back(data.first);
        for (auto &true_key : keys)
            this->cache.remove(this->cache.tree, true_key);
        result = this->cache.show_columns(this->cache.tree);
        break;
    }
    case Operation::UPDATE:
    {
        if (this->cache.table_name != statement.table)
            break;
        key = statement.datas[3];
        std::vector<std::shared_ptr<IndexTree::LeafNode>> leafs = this->cache.fuzzy_find_leaf(this->cache.tree, key);
        std::vector<std::string> keys;
        std::vector<std::vector<std::string>> new_datas;
        int index = 0;
        auto size = this->cache.tree->column.size();
        while (index < size && this->cache.tree->column[index] != statement.datas[2])
            index++;
        int index_new = 0;
        while (index_new < size && this->cache.tree->column[index_new] != statement.datas[0])
            index_new++;
        for (auto &leaf : leafs)
            for (auto &data : leaf->datas)
                // if (data.first.find(key) != std::string::npos)
                if (data.second[index].find(key) != std::string::npos)
                {
                    keys.push_back(data.first);
                    std::vector<std::string> tmp = data.second;
                    tmp[index_new] = statement.datas[1];
                    new_datas.push_back(tmp);
                }
        for (int i = 0; i < keys.size(); i++)
        {
            this->cache.remove(this->cache.tree, keys[i]);
            std::string new_key;
            for (auto &j : new_datas[i])
                new_key += j + ",";
            this->cache.insert(this->cache.tree, new_key, new_datas[i]);
        }
        result = this->cache.show_columns(this->cache.tree);
        break;
    }
    case Operation::SELECT:
    {
        if (this->cache.table_name != statement.table)
            break;
        key = statement.datas[2];
        std::vector<std::shared_ptr<IndexTree::LeafNode>> leafs = this->cache.fuzzy_find_leaf(this->cache.tree, key);
        int index = 0;
        auto size = this->cache.tree->column.size();
        while (index < size && this->cache.tree->column[index] != statement.datas[1])
            index++;
        int index_sel = -1;
        if (statement.datas[0] != "*")
        {
            index_sel = 0;
            while (index_sel < size && this->cache.tree->column[index_sel] != statement.datas[0])
                index_sel++;
        }
        int count = 0;
        for (auto &leaf : leafs)
            for (auto &data : leaf->datas)
                // if (data.first.find(key) != std::string::npos)
                if (data.second[index].find(key) != std::string::npos)
                {
                    if (index_sel == -1)
                        count++;
                    else
                        result.push_back(data.second[index_sel]);
                }
        if (index_sel == -1)
            result = {std::to_string(count)};
        break;
    }
    default:
        break;
    }
    return result;
}

void Runtime::scheduler()
{
    while (!fileOp.is_empty())
    {
        FileOperation::Task task = fileOp.consume_task();
        fileOp.work(task);
    }
}

bool Runtime::is_finish()
{
    return fileOp.is_empty();
}