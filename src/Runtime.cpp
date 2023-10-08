#include "..\include\Runtime.hpp"

bool Runtime::valid_statement(const Runtime::Statement &statement)
{
    return statement.opt != Runtime::Operation::ERRORP;
}

std::vector<std::string> Runtime::run_statement(const Runtime::Statement &statement)
{
    std::vector<std::string> result;
    std::string key;
    switch (statement.opt)
    {
        case Runtime::Operation::CREATE:
            Runtime::tree = IndexTree::build(statement.datas);
            result = IndexTree::show_columns(Runtime::tree);
            break;
        case Runtime::Operation::INSERT:
            for (auto &i : statement.datas)
                key += i;
            IndexTree::insert(Runtime::tree, key, statement.datas);
            result = IndexTree::show_columns(Runtime::tree);
            break;
        case Runtime::Operation::DELETE:
        {
            key = statement.datas[1];
            std::vector<std::shared_ptr<IndexTree::LeafNode>> leafs = IndexTree::fuzzy_find_leaf(Runtime::tree, key);
            std::vector<std::string> keys;
            int index = 0;
            auto size = Runtime::tree->column.size();
            while (index < size && Runtime::tree->column[index] != statement.datas[0])
                index++;
            for (auto &leaf : leafs)
                for (auto &data : leaf->datas)
                    if (data.first.find(key) != std::string::npos)
                        if (data.second[index].find(key) != std::string::npos)
                            keys.push_back(data.first);
            for (auto &true_key : keys)
                IndexTree::remove(Runtime::tree, true_key);
            result = IndexTree::show_columns(Runtime::tree);
            break;
        }
        case Runtime::Operation::UPDATE:
        {
            key = statement.datas[3];
            std::vector<std::shared_ptr<IndexTree::LeafNode>> leafs = IndexTree::fuzzy_find_leaf(Runtime::tree, key);
            std::vector<std::string> keys;
            std::vector<std::vector<std::string>> new_datas;
            int index = 0;
            auto size = Runtime::tree->column.size();
            while (index < size && Runtime::tree->column[index] != statement.datas[2])
                index++;
            int index_new = 0;
            while (index_new < size && Runtime::tree->column[index_new] != statement.datas[0])
                index_new++;
            for (auto &leaf : leafs)
                for (auto &data : leaf->datas)
                    if (data.first.find(key) != std::string::npos)
                        if (data.second[index].find(key) != std::string::npos)
                        {
                            keys.push_back(data.first);
                            std::vector<std::string> tmp = data.second;
                            tmp[index_new] = statement.datas[1];
                            new_datas.push_back(tmp);
                        }
            for (int i = 0; i < keys.size(); i++)
            {
                IndexTree::remove(Runtime::tree, keys[i]);
                std::string new_key;
                for (auto &j : new_datas[i])
                    new_key += j;
                IndexTree::insert(Runtime::tree, new_key, new_datas[i]);
            }
            result = IndexTree::show_columns(Runtime::tree);
            break;
        }
        case Runtime::Operation::SELECT:
        {
            key = statement.datas[1];
            std::vector<std::shared_ptr<IndexTree::LeafNode>> leafs = IndexTree::fuzzy_find_leaf(Runtime::tree, key);
            int index = 0;
            auto size = Runtime::tree->column.size();
            while (index < size && Runtime::tree->column[index] != statement.datas[0])
                index++;
            for (auto &leaf : leafs)
                for (auto &data : leaf->datas)
                    if (data.first.find(key) != std::string::npos)
                        if (data.second[index].find(key) != std::string::npos)
                        {
                            std::string tmp;
                            for (auto &j : data.second)
                                tmp += j + " ";
                            result.push_back(tmp);
                        }
            break;
        }
        default:
            break;
    }
    return result;
}