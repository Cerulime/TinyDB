#include "..\include\IndexTree.hpp"

std::shared_ptr<IndexTree::Tree> IndexTree::build()
{
    std::shared_ptr<IndexTree::Tree> tree = std::make_shared<IndexTree::Tree>();
    tree->root = std::make_shared<LeafNode>();
    tree->seed = HashTable::get_rand();
    return tree;
}

void IndexTree::split(std::shared_ptr<IndexTree::Node> node)
{
    auto father = node->parent;
    if (father == nullptr)
    {
        auto internal = std::make_shared<IndexTree::InternalNode>();
        internal->is_leaf = false;
        internal->parent = nullptr;
        auto leaf = std::dynamic_pointer_cast<IndexTree::LeafNode>(node);
        auto new_leaf = std::make_shared<IndexTree::LeafNode>();
        new_leaf->is_leaf = true;
        new_leaf->parent = internal;
        new_leaf->next = leaf->next;
        leaf->next = new_leaf;
        new_leaf->datas.assign(leaf->datas.begin() + IndexTree::max_children / 2, leaf->datas.end());
        leaf->datas.erase(leaf->datas.begin() + IndexTree::max_children / 2, leaf->datas.end());
        internal->children[leaf->datas.back()] = leaf;
        internal->children[new_leaf->datas.back()] = new_leaf;
        leaf->parent = internal;
        node->parent = internal;
        node = internal;
        return;
    }
}

void IndexTree::insert(std::shared_ptr<IndexTree::Tree> tree, const std::string &key)
{
    if (tree == nullptr)
        tree = build();
    auto now = IndexTree::find_leaf(tree, key);
    if (now == nullptr)
    {
        auto internal = std::dynamic_pointer_cast<IndexTree::InternalNode>(tree->root);
        while (true)
        {
            auto last = (internal->children.rbegin());
            auto child = last->second;
            internal->children.erase((++last).base());
            internal->children[key]=child;
            if (child->is_leaf)
            {
                now = std::dynamic_pointer_cast<IndexTree::LeafNode>(child);
                break;
            }
            internal = std::dynamic_pointer_cast<IndexTree::InternalNode>(child);
        }
    }
    auto it = std::upper_bound(now->datas.begin(), now->datas.end(), key);
    if (it == now->datas.end())
        now->datas.push_back(key);
    else return;
    std::sort(now->datas.begin(), now->datas.end());
    if (now->datas.size() == IndexTree::max_children)
        IndexTree::split(now);
    return;
}

std::shared_ptr<IndexTree::LeafNode> IndexTree::find_leaf(std::shared_ptr<IndexTree::Tree> tree, const std::string &key)
{
    if (tree->root->is_leaf)
        return std::dynamic_pointer_cast<IndexTree::LeafNode>(tree->root);
    auto now = std::dynamic_pointer_cast<IndexTree::InternalNode>(tree->root);
    while (true)
    {
        auto child = now->children.upper_bound(key);
        if (child == now->children.end())
            return nullptr;
        auto next = child->second;
        if (next->is_leaf)
            return std::dynamic_pointer_cast<IndexTree::LeafNode>(next);
        now = std::dynamic_pointer_cast<IndexTree::InternalNode>(next);
    }
    return nullptr;
}

std::vector<std::shared_ptr<IndexTree::LeafNode>> IndexTree::fuzzy_find_leaf(std::shared_ptr<IndexTree::Tree> tree, const std::string &key)
{
    return {};
}