#pragma once
#include "HashTable.hpp"

namespace IndexTree
{
    inline const int max_children = 64;
    struct Node
    {
        bool is_leaf;
        std::shared_ptr<Node> parent;
        std::bitset<HashTable::prime> map;
        explicit Node(bool is_leaf) : is_leaf(is_leaf) {}
        virtual ~Node() = default;
    };
    struct LeafNode : Node
    {
        std::string filename;
        std::shared_ptr<LeafNode> prev, next;
        std::vector<std::string> datas;
        LeafNode() : Node(true) {}
    };
    struct InternalNode : Node
    {
        std::map<std::string, std::shared_ptr<Node>> children;
        InternalNode() : Node(false) {}
    };
    struct Tree
    {
        std::shared_ptr<Node> root;
        unsigned long long seed;
    };

    std::shared_ptr<Tree> build();
    void merge_map(std::shared_ptr<InternalNode>);
    std::shared_ptr<InternalNode> split(std::shared_ptr<Node>, const unsigned long long &);
    void insert(std::shared_ptr<Tree>, const std::string &);
    std::shared_ptr<LeafNode> find_leaf(std::shared_ptr<Tree>, const std::string &);
    std::vector<std::shared_ptr<LeafNode>> fuzzy_find_leaf(std::shared_ptr<Tree>, const std::string &);
    bool in_leaf(std::shared_ptr<LeafNode>, const std::string &);
    void remove(std::shared_ptr<Tree>, const std::string &);
}