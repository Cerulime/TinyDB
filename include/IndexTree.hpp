#pragma once
#include "HashTable.hpp"

namespace IndexTree
{
    inline const int max_children = 64;

    struct InternalNode;
    struct Node
    {
        bool is_leaf;
        std::shared_ptr<InternalNode> parent;
        std::bitset<HashTable::PRIME> map;
        explicit Node(bool is_leaf) : is_leaf(is_leaf) {}
        virtual ~Node() = default;
    };
    struct LeafNode : Node
    {
        std::string filename;
        std::shared_ptr<LeafNode> next;
        std::map<std::string, std::vector<std::string>>datas;
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
        int cnt;
        std::vector<std::string> column;
    };

    std::shared_ptr<Tree> build(const std::vector<std::string> &);
    std::vector<std::string> show_columns(const std::shared_ptr<Tree> &);
    void merge_map(std::shared_ptr<LeafNode>, const unsigned long long &);
    void merge_map(std::shared_ptr<InternalNode>);
    void update_map(std::shared_ptr<InternalNode>);
    std::shared_ptr<InternalNode> split(std::shared_ptr<Node>, const unsigned long long &);
    void insert(std::shared_ptr<Tree>, const std::string &, const std::vector<std::string> &);
    std::shared_ptr<LeafNode> find_leaf(std::shared_ptr<Tree>, const std::string &);
    std::vector<std::shared_ptr<LeafNode>> fuzzy_find_leaf(std::shared_ptr<Tree>, const std::string &);
    inline bool in_leaf(std::shared_ptr<LeafNode>, const std::string &);
    void merge(std::shared_ptr<Node>, const unsigned long long &);
    void remove(std::shared_ptr<Tree>, const std::string &);
}