#include "..\include\Runtime.hpp"

bool Runtime::valid_statement(const Statement &statement)
{
    return statement.opt != Operation::ERRORP;
}

std::vector<std::string> Runtime::run_statement(const Statement &statement)
{
    std::vector<std::string> result;
    std::string key;
    switch (statement.opt)
    {
        case Operation::CREATE:
            tree = IndexTree::build(statement.datas);
            result = IndexTree::show_columns(tree);
            break;
        case Operation::INSERT:
            for (auto &i : statement.datas)
                key += i;
            IndexTree::insert(tree, key, statement.datas);
            result = IndexTree::show_columns(tree);
            break;
        case Operation::DELETE:
        {
            key = statement.datas[1];
            std::vector<std::shared_ptr<IndexTree::LeafNode>> leafs = IndexTree::fuzzy_find_leaf(tree, key);
            std::vector<std::string> keys;
            int index = 0;
            auto size = tree->column.size();
            while (index < size && tree->column[index] != statement.datas[0])
                index++;
            for (auto &leaf : leafs)
                for (auto &data : leaf->datas)
                    if (data.first.find(key) != std::string::npos)
                        if (data.second[index].find(key) != std::string::npos)
                            keys.push_back(data.first);
            for (auto &true_key : keys)
                IndexTree::remove(tree, true_key);
            result = IndexTree::show_columns(tree);
            break;
        }
        case Operation::UPDATE:
        {
            key = statement.datas[3];
            std::vector<std::shared_ptr<IndexTree::LeafNode>> leafs = IndexTree::fuzzy_find_leaf(tree, key);
            std::vector<std::string> keys;
            std::vector<std::vector<std::string>> new_datas;
            int index = 0;
            auto size = tree->column.size();
            while (index < size && tree->column[index] != statement.datas[2])
                index++;
            int index_new = 0;
            while (index_new < size && tree->column[index_new] != statement.datas[0])
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
                IndexTree::remove(tree, keys[i]);
                std::string new_key;
                for (auto &j : new_datas[i])
                    new_key += j;
                IndexTree::insert(tree, new_key, new_datas[i]);
            }
            result = IndexTree::show_columns(tree);
            break;
        }
        case Operation::SELECT:
        {
            key = statement.datas[1];
            std::vector<std::shared_ptr<IndexTree::LeafNode>> leafs = IndexTree::fuzzy_find_leaf(tree, key);
            int index = 0;
            auto size = tree->column.size();
            while (index < size && tree->column[index] != statement.datas[0])
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