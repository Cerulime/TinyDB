#include "HashTable.hpp"

namespace IndexTree
{
    const int max_children = 16;
    struct Node
    {
        bool is_leaf;
        std::shared_ptr<Node> parent;
        virtual ~Node() = default;
    };
    struct LeafNode : Node
    {
        std::string filename;
        std::shared_ptr<LeafNode> next;
        std::vector<std::string> datas;
    };
    struct InternalNode : Node
    {
        std::map<std::string, std::shared_ptr<Node>> children;
        std::bitset<HashTable::prime> map;
    };
    struct Tree
    {
        std::shared_ptr<Node> root;
        unsigned long long seed;
    };

    std::shared_ptr<Tree> build();
    void split(std::shared_ptr<Node>);
    void insert(std::shared_ptr<Tree>, const std::string &);
    std::shared_ptr<LeafNode> find_leaf(std::shared_ptr<Tree>, const std::string &);
    std::vector<std::shared_ptr<LeafNode>> fuzzy_find_leaf(std::shared_ptr<Tree>, const std::string &);
}