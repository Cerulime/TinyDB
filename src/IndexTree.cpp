#include "..\include\IndexTree.hpp"
#include "..\include\FileOperation.hpp"

/**
 * @brief Builds a new IndexTree.
 * 
 * @return std::shared_ptr<IndexTree::Tree> A shared pointer to the newly created IndexTree.
 */
std::shared_ptr<IndexTree::Tree> IndexTree::build()
{
    std::shared_ptr<IndexTree::Tree> tree = std::make_shared<IndexTree::Tree>();
    tree->root = std::make_shared<LeafNode>();
    tree->seed = HashTable::get_rand();
    return tree;
}

/**
 * @brief Merge the map of a leaf node with a given seed.
 * 
 * @param node The leaf node to merge its map.
 * @param seed The seed to use for creating the map.
 */
void IndexTree::merge_map(std::shared_ptr<IndexTree::LeafNode> node, const unsigned long long &seed)
{
    node->map.reset();
    for (const auto &[key, data] : node->datas)
    {
        auto hash = HashTable::create_map(key, seed);
        for (const auto &pos : hash)
            node->map[pos] = true;
    }
}

/**
 * Merges the bitmaps of all children nodes of the given internal node and sets the merged bitmap to the node's map.
 * If the node's children are leaf nodes, the bitmaps of the leaf nodes are merged. Otherwise, the bitmaps of the internal nodes are merged.
 * @param node A shared pointer to the internal node whose children's bitmaps are to be merged.
 */
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

/**
 * @brief Updates the map of each ancestor of the given node by merging the maps of its children.
 * 
 * @param ancestor A shared pointer to the ancestor node whose map needs to be updated.
 */
void IndexTree::update_map(std::shared_ptr<IndexTree::InternalNode> ancestor)
{
    while (ancestor != nullptr)
    {
        IndexTree::merge_map(ancestor);
        ancestor = ancestor->parent;
    }
}

/**
 * Splits a node in the index tree and returns the new internal node.
 * If the node is a leaf node, it splits the leaf node into two leaf nodes and returns the new internal node.
 * If the node is an internal node, it splits the internal node into two internal nodes and returns the new internal node.
 * @param node The node to be split.
 * @param seed The seed used for random number generation.
 * @return The new internal node.
 */
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

        auto ptr = now->datas.begin();
        std::advance(ptr, now->datas.size() / 2);
        while (ptr != now->datas.end())
        {
            new_leaf->datas.insert(std::move(*ptr));
            ptr = now->datas.erase(ptr);
        }
        IndexTree::merge_map(new_leaf, seed);
        IndexTree::merge_map(now, seed);

        new_leaf->parent = father;
        new_leaf->filename = now->filename;

        if (father == nullptr)
        {
            auto new_father = std::make_shared<IndexTree::InternalNode>();
            new_father->children[now->datas.rbegin()->first] = now;
            new_father->children[new_leaf->datas.rbegin()->first] = new_leaf;
            now->parent = new_father;
            new_leaf->parent = new_father;
            new_father->map = now->map | new_leaf->map;
            return new_father;
        }
        else
        {
            auto old_father = std::dynamic_pointer_cast<IndexTree::InternalNode>(father);
            old_father->children[now->datas.rbegin()->first] = now;
            old_father->children[new_leaf->datas.rbegin()->first] = new_leaf;
            if (old_father->children.size() > IndexTree::max_children)
                return IndexTree::split(old_father, seed);
            IndexTree::update_map(old_father);
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
        IndexTree::merge_map(now);
        IndexTree::merge_map(new_internal);

        new_internal->parent = father;

        if (father == nullptr)
        {
            auto new_father = std::make_shared<IndexTree::InternalNode>();
            new_father->children[now->children.rbegin()->first] = now;
            new_father->children[new_internal->children.rbegin()->first] = new_internal;
            now->parent = new_father;
            new_internal->parent = new_father;
            new_father->map = now->map | new_internal->map;
            return new_father;
        }
        else
        {
            auto old_father = std::dynamic_pointer_cast<IndexTree::InternalNode>(father);
            old_father->children[now->children.rbegin()->first] = now;
            old_father->children[new_internal->children.rbegin()->first] = new_internal;
            if (old_father->children.size() > IndexTree::max_children)
                return IndexTree::split(old_father, seed);
            IndexTree::update_map(old_father);
            return old_father;
        }
    }
}

/**
 * Checks if the given key exists in the leaf node.
 * @param leaf The leaf node to search in.
 * @param key The key to search for.
 * @return True if the key exists in the leaf node, false otherwise.
 */
inline bool IndexTree::in_leaf(std::shared_ptr<IndexTree::LeafNode> leaf, const std::string &key)
{
    return leaf->datas.find(key) != leaf->datas.end();
}

/**
 * Inserts a new key-value pair into the index tree.
 * @param tree A shared pointer to the tree to insert into.
 * @param key The key to insert.
 * @param data The data to associate with the key.
 */
void IndexTree::insert(std::shared_ptr<IndexTree::Tree> tree, const std::string &key, const std::vector<std::pair<std::string, std::string>> &data)
{
    if (tree == nullptr)
        tree = build();
    auto now = IndexTree::find_leaf(tree, key);
    assert(now != nullptr);
    if (IndexTree::in_leaf(now, key))
        return;
    if (!tree->root->is_leaf && key > now->datas.rbegin()->first)
    {
        auto internal = std::dynamic_pointer_cast<IndexTree::InternalNode>(tree->root);
        while (true)
        {
            auto last = internal->children.rbegin();
            auto child = last->second;
            internal->children.erase((++last).base());
            internal->children[key]=child;
            if (child->is_leaf)
                break;
            internal = std::dynamic_pointer_cast<IndexTree::InternalNode>(child);
        }
    }
    now->datas[key] = data;
    IndexTree::merge_map(now, tree->seed);
    if (now->datas.size() == IndexTree::max_children)
    {
        auto old_root = tree->root;
        auto new_parent = IndexTree::split(now, tree->seed);
        if (old_root != tree->root)
            tree->root = new_parent;
        return;
    }
    IndexTree::update_map(now->parent);
    return;
}

/**
 * Finds the leaf node in the tree that contains the given key.
 * @param tree The tree to search in.
 * @param key The key to search for.
 * @return A shared pointer to the leaf node that contains the key, or nullptr if the key is not found.
 */
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

/**
 * Searches for all leaf nodes in the given tree that contain the given key.
 * @param tree A shared pointer to the tree to search in.
 * @param key The key to search for.
 * @return A vector of shared pointers to the leaf nodes that contain the key.
 */
std::vector<std::shared_ptr<IndexTree::LeafNode>> IndexTree::fuzzy_find_leaf(std::shared_ptr<IndexTree::Tree> tree, const std::string &key)
{
    auto hash = HashTable::create_map(key, tree->seed);
    auto is_all_inmap = [&hash](const std::bitset<HashTable::prime> &map) -> bool {
        for (const auto &pos : hash)
            if (!map[pos])
                return false;
        return true;
    };
    std::vector<std::shared_ptr<IndexTree::LeafNode>> ret;

    if (tree->root->is_leaf)
    {
        auto tmp = std::dynamic_pointer_cast<IndexTree::LeafNode>(tree->root);
        if (is_all_inmap(tmp->map))
            ret.push_back(tmp);
        return ret;
    }
    auto tmp = std::dynamic_pointer_cast<IndexTree::InternalNode>(tree->root);
    if (!is_all_inmap(tmp->map))
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
                if (is_all_inmap(leaf->map))
                    ret.push_back(std::move(leaf));
                continue;
            }
            auto internal = std::dynamic_pointer_cast<IndexTree::InternalNode>(child);
            if (is_all_inmap(internal->map))
                internal_nodes_queue.push(std::move(internal));
        }
    }
    return ret;
}

/**
 * Merges the given node with its sibling if necessary, and updates the index tree accordingly.
 * 
 * @param node The node to be merged.
 * @param seed The seed used for random number generation.
 */
void IndexTree::merge(std::shared_ptr<IndexTree::Node> node, const unsigned long long &seed)
{
    auto father = node->parent;
    if (father == nullptr)
        return;
    if (node->is_leaf)
    {
        if (father->children.size() == 1)
            return;
        auto now = std::dynamic_pointer_cast<IndexTree::LeafNode>(node);
        auto get_brother = [](std::shared_ptr<IndexTree::LeafNode> now)->std::shared_ptr<IndexTree::LeafNode> {
            auto father = now->parent;
            auto it = std::prev(father->children.find(now->datas.rbegin()->first));
            if (it == father->children.begin())
                return nullptr;
            return std::dynamic_pointer_cast<IndexTree::LeafNode>(it->second);
        };
        
        auto brother = get_brother(now);
        if (brother == nullptr)
        {
            auto father = now->parent;
            auto it = father->children.find(now->datas.rbegin()->first);
            if (it == father->children.begin())
            {
                auto cousin = std::dynamic_pointer_cast<IndexTree::LeafNode>(now->next);
                cousin->datas.merge(now->datas);
                now->datas.clear();
                FileOperation::merge_file(cousin->filename, now->filename);
                father->children.erase(it);
                IndexTree::merge_map(cousin, seed);
                IndexTree::merge_map(father);
                if (father->children.size() < IndexTree::max_children / 2)
                    return IndexTree::merge(father, seed);
                IndexTree::update_map(father->parent);
            }
            return;
        }
        if (brother->datas.size() - 1 >= IndexTree::max_children / 2)
        {
            auto max = brother->datas.rbegin();
            father->children.erase(max->first);
            now->datas.insert(std::move(*max));
            brother->datas.erase(max->first);
            father->children[brother->datas.rbegin()->first] = brother;
            IndexTree::merge_map(now, seed);
            IndexTree::merge_map(brother, seed);
            IndexTree::update_map(father);
            return;
        }
        auto cousin = get_brother(brother);
        if (cousin != nullptr)
            cousin->next = now;
        auto max = brother->datas.rbegin();
        father->children.erase(max->first);
        now->datas.merge(brother->datas);
        brother->datas.clear();
        FileOperation::merge_file(now->filename, brother->filename);
        IndexTree::merge_map(now, seed);
        IndexTree::merge_map(father);
        if (father->children.size() < IndexTree::max_children / 2)
            return IndexTree::merge(father, seed);
        IndexTree::update_map(father->parent);
    }
    else
    {
        auto now = std::dynamic_pointer_cast<IndexTree::InternalNode>(node);
        auto get_brother = [](std::shared_ptr<IndexTree::InternalNode> now)->std::shared_ptr<IndexTree::InternalNode> {
            auto father = now->parent;
            auto it = std::prev(father->children.find(now->children.rbegin()->first));
            if (it == father->children.begin())
                return nullptr;
            return std::dynamic_pointer_cast<IndexTree::InternalNode>(it->second);
        };

        auto brother = get_brother(now);
        if (brother == nullptr)
        {
            auto father = now->parent;
            auto it = father->children.find(now->children.rbegin()->first);
            if (it == father->children.begin())
            {
                auto cousin = std::dynamic_pointer_cast<IndexTree::InternalNode>(now->children.begin()->second);
                cousin->children.merge(now->children);
                now->children.clear();
                father->children.erase(it);
                IndexTree::merge_map(cousin);
                IndexTree::merge_map(father);
                if (father->children.size() < IndexTree::max_children / 2)
                    return IndexTree::merge(father, seed);
                IndexTree::update_map(father->parent);
            }
            return;
        }
        if (brother->children.size() - 1 >= IndexTree::max_children / 2)
        {
            auto max = brother->children.rbegin();
            father->children.erase(max->first);
            now->children.insert(std::move(*max));
            brother->children.erase(max->first);
            father->children[brother->children.rbegin()->first] = brother;
            IndexTree::merge_map(now);
            IndexTree::merge_map(brother);
            IndexTree::update_map(father);
            return;
        }
        auto max = brother->children.rbegin();
        father->children.erase(max->first);
        now->children.merge(brother->children);
        brother->children.clear();
        IndexTree::merge_map(now);
        IndexTree::merge_map(father);
        if (father->children.size() < IndexTree::max_children / 2)
            return IndexTree::merge(father, seed);
        IndexTree::update_map(father->parent);
    }
}

/**
 * Removes the node with the given key from the tree.
 * @param tree A shared pointer to the tree.
 * @param key The key of the node to be removed.
 */
void IndexTree::remove(std::shared_ptr<IndexTree::Tree> tree, const std::string &key)
{
    auto now = IndexTree::find_leaf(tree, key);
    assert(now != nullptr);
    if (!IndexTree::in_leaf(now, key))
        return;
    if (now->datas.size() == 1)
    {
        if (now == tree->root)
        {
            tree->root = nullptr;
            return;
        }
        auto father = now->parent;
        father->children.erase(key);
        IndexTree::merge_map(father);
        if (father->children.size() < IndexTree::max_children / 2)
            return IndexTree::merge(father, tree->seed);
        IndexTree::update_map(father->parent);
        return;
    }
    if (!tree->root->is_leaf && key == now->datas.rbegin()->first)
    {
        auto internal = std::dynamic_pointer_cast<IndexTree::InternalNode>(tree->root);
        auto second_last = (++now->datas.rbegin())->first;
        while (true)
        {
            auto last = internal->children.rbegin();
            auto child = last->second;
            internal->children.erase((++last).base());
            internal->children[second_last]=child;
            if (child->is_leaf)
                break;
            internal = std::dynamic_pointer_cast<IndexTree::InternalNode>(child);
        }
    }
    now->datas.erase(key);
    IndexTree::merge_map(now, tree->seed);
    if (now->datas.size() < IndexTree::max_children / 2)
    {
        IndexTree::merge(now, tree->seed);
        if (!tree->root->is_leaf)
        {
            auto root = std::dynamic_pointer_cast<InternalNode>(tree->root);
            if (root->children.size() == 1)
            {
                auto child = root->children.begin()->second;
                tree->root = child;
                child->parent = nullptr;
            }
        }
        return;
    }
    IndexTree::update_map(now->parent);
    return;
}