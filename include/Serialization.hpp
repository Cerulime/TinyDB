#pragma once
#include "IndexTree.hpp"

namespace Serialization
{
    inline const char base64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    std::string base64encode(const char*, const std::size_t &);
    void base64decode(const std::string &, char*, const std::size_t &);
    std::string serialize_map(const std::bitset<HashTable::PRIME> &);
    std::bitset<HashTable::PRIME> deserialize_map(const std::string &);
    std::string serialize_tree(const std::shared_ptr<IndexTree::Tree> &);
    std::shared_ptr<IndexTree::Tree> deserialize_tree(const std::string &);
}