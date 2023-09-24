#include "..\include\IndexTree.hpp"

std::shared_ptr<IndexTree::Tree> IndexTree::build()
{
    std::shared_ptr<IndexTree::Tree> tree = std::make_shared<IndexTree::Tree>();
    tree->root = std::make_shared<LeafNode>();
    tree->seed = HashTable::get_rand();
    return tree;
}

void IndexTree::merge_map(std::shared_ptr<IndexTree::InternalNode> node)
{
    node->map.reset();
    const bool &is_leaf = node->children.begin()->second->is_leaf;
    for (const auto &[key, child] : node->children)
    {
        if (is_leaf)
            node->map |= std::dynamic_pointer_cast<IndexTree::LeafNode>(child)->map;
        else
            node->map |= std::dynamic_pointer_cast<IndexTree::InternalNode>(child)->map;
    }
}

std::shared_ptr<IndexTree::InternalNode> IndexTree::split(std::shared_ptr<IndexTree::Node> node, const unsigned long long &seed)
{
    auto father = node->parent;
    if (node->is_leaf)
    {
        auto now = std::dynamic_pointer_cast<IndexTree::LeafNode>(node);
        auto new_leaf = std::make_shared<IndexTree::LeafNode>();
        auto old_next_leaf = now->next;
        now->next = new_leaf;
        new_leaf->next = old_next_leaf;

        new_leaf->datas.insert(new_leaf->datas.end(), std::make_move_iterator(now->datas.begin() + now->datas.size() / 2), std::make_move_iterator(now->datas.end()));
        now->datas.erase(now->datas.begin() + now->datas.size() / 2, now->datas.end());
        auto update = [&seed](std::shared_ptr<IndexTree::LeafNode> now){
            now->map.reset();
            for (const auto &data : now->datas)
            {
                auto hash = HashTable::create_map(data, seed);
                for (const auto &pos : hash)
                    now->map[pos] = true;
            }
        };
        update(new_leaf);
        update(now);

        new_leaf->parent = father;
        new_leaf->filename = now->filename;

        if (father == nullptr)
        {
            auto new_father = std::make_shared<IndexTree::InternalNode>();
            new_father->children[now->datas.back()] = now;
            new_father->children[new_leaf->datas.back()] = new_leaf;
            now->parent = new_father;
            new_leaf->parent = new_father;
            return new_father;
        }
        else
        {
            auto old_father = std::dynamic_pointer_cast<IndexTree::InternalNode>(father);
            old_father->children[now->datas.back()] = now;
            old_father->children[new_leaf->datas.back()] = new_leaf;
            return old_father;
        }
    }
    else
    {
        auto now = std::dynamic_pointer_cast<IndexTree::InternalNode>(node);
        auto new_internal = std::make_shared<IndexTree::InternalNode>();

        auto ptr = now->children.begin();
        std::advance(ptr, now->children.size() / 2);
        while (ptr != now->children.end())
        {
            new_internal->children.insert(std::move(*ptr));
            ptr = now->children.erase(ptr);
        }
        new_internal->parent = father;
        IndexTree::merge_map(now);
        IndexTree::merge_map(new_internal);

        if (father == nullptr)
        {
            auto new_father = std::make_shared<IndexTree::InternalNode>();
            new_father->children[now->children.rbegin()->first] = now;
            new_father->children[new_internal->children.rbegin()->first] = new_internal;
            now->parent = new_father;
            new_internal->parent = new_father;
            return new_father;
        }
        else
        {
            auto old_father = std::dynamic_pointer_cast<IndexTree::InternalNode>(father);
            old_father->children[now->children.rbegin()->first] = now;
            old_father->children[new_internal->children.rbegin()->first] = new_internal;
            return old_father;
        }
    }
}

bool IndexTree::in_leaf(std::shared_ptr<IndexTree::LeafNode> leaf, const std::string &key)
{
    auto it = std::lower_bound(leaf->datas.begin(), leaf->datas.end(), key);
    if (it == leaf->datas.end())
        return false;
    return *it == key;
}

void IndexTree::insert(std::shared_ptr<IndexTree::Tree> tree, const std::string &key)
{
    if (tree == nullptr)
        tree = build();
    auto now = IndexTree::find_leaf(tree, key);
    assert(now != nullptr);
    if (IndexTree::in_leaf(now, key))
        return;
    if (!tree->root->is_leaf && key > now->datas.back())
    {
        auto internal = std::dynamic_pointer_cast<IndexTree::InternalNode>(tree->root);
        while (true)
        {
            auto last = (internal->children.rbegin());
            auto child = last->second;
            internal->children.erase((++last).base());
            internal->children[key]=child;
            if (child->is_leaf)
                break;
            internal = std::dynamic_pointer_cast<IndexTree::InternalNode>(child);
        }
    }
    now->datas.push_back(key);
    std::sort(now->datas.begin(), now->datas.end());
    if (now->datas.size() == IndexTree::max_children)
    {
        bool modify_root = tree->root == now;
        auto new_parent = IndexTree::split(now, tree->seed);
        if (modify_root)
            tree->root = new_parent;
    }
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
            child--;
        auto next = child->second;
        if (next->is_leaf)
            return std::dynamic_pointer_cast<IndexTree::LeafNode>(next);
        now = std::dynamic_pointer_cast<IndexTree::InternalNode>(next);
    }
    return nullptr;
}

std::vector<std::shared_ptr<IndexTree::LeafNode>> IndexTree::fuzzy_find_leaf(std::shared_ptr<IndexTree::Tree> tree, const std::string &key)
{
    auto hash = HashTable::create_map(key, tree->seed);
    auto is_in = [&hash](const std::bitset<HashTable::prime> &map) -> bool {
        for (const auto &pos : hash)
            if (!map[pos])
                return false;
        return true;
    };
    std::vector<std::shared_ptr<IndexTree::LeafNode>> ret;

    if (tree->root->is_leaf)
    {
        auto tmp = std::dynamic_pointer_cast<IndexTree::LeafNode>(tree->root);
        if (is_in(tmp->map))
            ret.push_back(tmp);
        return ret;
    }
    auto tmp = std::dynamic_pointer_cast<IndexTree::InternalNode>(tree->root);
    if (!is_in(tmp->map))
        return ret;
    std::queue<std::shared_ptr<IndexTree::InternalNode>> internal_nodes_queue;
    internal_nodes_queue.push(std::move(tmp));
    while (!internal_nodes_queue.empty())
    {
        auto now = internal_nodes_queue.front();
        internal_nodes_queue.pop();
        for (const auto &[key, child] : now->children)
        {
            if (child->is_leaf)
            {
                auto leaf = std::dynamic_pointer_cast<IndexTree::LeafNode>(child);
                if (is_in(leaf->map))
                    ret.push_back(std::move(leaf));
                continue;
            }
            auto internal = std::dynamic_pointer_cast<IndexTree::InternalNode>(child);
            if (is_in(internal->map))
                internal_nodes_queue.push(std::move(internal));
        }
    }
    return ret;
}