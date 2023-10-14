#include "..\include\IndexTree.hpp"
#include "..\include\FileOperation.hpp"

bool IndexTree::check_tree()
{
    if (this->tree->root->is_leaf)
        return true;
    std::queue<std::shared_ptr<InternalNode>> q;
    q.push(std::dynamic_pointer_cast<InternalNode>(this->tree->root));
    while (!q.empty())
    {
        auto now = q.front();
        q.pop();
        for (auto &[key, child] : now->children)
        {
            if (child->is_leaf)
            {
                auto leaf = std::dynamic_pointer_cast<LeafNode>(child);
                if (leaf->parent != now)
                    return false;
                if (leaf->datas.rbegin()->first != key)
                    return false;
                if (leaf->datas.size() < max_children / 2)
                    return false;
            }
            else
            {
                auto internal = std::dynamic_pointer_cast<InternalNode>(child);
                if (internal->parent != now)
                    return false;
                if (internal->children.rbegin()->first != key)
                    return false;
                if (internal->children.size() < max_children / 2)
                    return false;
                q.push(internal);
            }
        }
    }
    return true;
}

/**
 * @brief Builds an IndexTree with the given column.
 *
 * @param column The column to build the IndexTree with.
 * @return std::shared_ptr<IndexTree::Tree> A shared pointer to the built IndexTree.
 */
std::shared_ptr<IndexTree::Tree> IndexTree::build(const std::vector<std::string> &column)
{
    std::shared_ptr<Tree> tree = std::make_shared<Tree>();
    tree->root = std::make_shared<LeafNode>(this->filename_map, this->hash_table);
    tree->seed = this->hash_table.get_rand();
    tree->column = column;
    return tree;
}

/**
 * Returns a vector of strings containing the column names and the number of rows in the given tree.
 * @param tree A shared pointer to the tree whose columns are to be shown.
 * @return A vector of strings containing the column names and the number of rows in the given tree.
 */
std::vector<std::string> IndexTree::show_columns(const std::shared_ptr<Tree> &tree)
{
    std::vector<std::string> ret;
    ret.push_back("Table name: " + this->table_name);
    std::string col;
    for (auto &x : tree->column)
        col += x + " ";
    ret.push_back(col);
    ret.push_back(std::to_string(tree->cnt) + " rows in set.");
    return ret;
}

/**
 * @brief Merge the map of a leaf node with a given seed.
 *
 * @param node The leaf node to merge the map.
 * @param seed The seed used to create the map.
 */
void IndexTree::merge_map(std::shared_ptr<LeafNode> node, const unsigned long long &seed)
{
    node->map.reset();
    for (const auto &[key, data] : node->datas)
        node->map |= this->hash_table.create_map(key, seed);
}

/**
 * Merges the bitmaps of all children of the given internal node into the node's own bitmap.
 * If the node's children are leaf nodes, their bitmaps are ORed together.
 * If the node's children are internal nodes, their bitmaps are ORed together.
 * @param node The internal node whose children's bitmaps are to be merged.
 */
void IndexTree::merge_map(std::shared_ptr<InternalNode> node)
{
    node->map.reset();
    const bool &is_leaf = node->children.begin()->second->is_leaf;
    if (is_leaf)
        for (const auto &[key, child] : node->children)
            node->map |= std::dynamic_pointer_cast<LeafNode>(child)->map;
    else
        for (const auto &[key, child] : node->children)
            node->map |= std::dynamic_pointer_cast<InternalNode>(child)->map;
}

/**
 * @brief Updates the map of each ancestor of the given node by merging the maps of its children.
 *
 * @param ancestor A shared pointer to the ancestor node to update the map for.
 */
void IndexTree::update_map(std::shared_ptr<InternalNode> ancestor)
{
    while (ancestor != nullptr)
    {
        merge_map(ancestor);
        ancestor = ancestor->parent;
    }
}

/**
 * Splits a node into two nodes when it is full and updates the parent node accordingly.
 * @param node The node to be split.
 * @param seed The seed used for random number generation.
 * @return A shared pointer to the new parent node if the original parent node was null, otherwise a shared pointer to the original parent node.
 */
std::shared_ptr<IndexTree::InternalNode> IndexTree::split(std::shared_ptr<Node> node, const unsigned long long &seed)
{
    auto father = node->parent;
    if (node->is_leaf)
    {
        auto now = std::dynamic_pointer_cast<LeafNode>(node);
        auto new_leaf = std::make_shared<LeafNode>(this->filename_map, this->hash_table);
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
        merge_map(new_leaf, seed);
        merge_map(now, seed);

        new_leaf->parent = father;
        // TODO: FileOperation
        if (father == nullptr)
        {
            auto new_father = std::make_shared<InternalNode>();
            new_father->children[now->datas.rbegin()->first] = now;
            new_father->children[new_leaf->datas.rbegin()->first] = new_leaf;
            now->parent = new_father;
            new_leaf->parent = new_father;
            new_father->map = now->map | new_leaf->map;
            return new_father;
        }
        else
        {
            auto old_father = std::dynamic_pointer_cast<InternalNode>(father);
            old_father->children[now->datas.rbegin()->first] = now;
            old_father->children[new_leaf->datas.rbegin()->first] = new_leaf;
            if (old_father->children.size() > max_children)
                return split(old_father, seed);
            update_map(old_father);
            return old_father;
        }
    }
    else
    {
        auto now = std::dynamic_pointer_cast<InternalNode>(node);
        auto new_internal = std::make_shared<InternalNode>();

        auto ptr = now->children.begin();
        std::advance(ptr, now->children.size() / 2);
        while (ptr != now->children.end())
        {
            ptr->second->parent = new_internal;
            new_internal->children.insert(std::move(*ptr));
            ptr = now->children.erase(ptr);
        }
        merge_map(now);
        merge_map(new_internal);

        new_internal->parent = father;

        if (father == nullptr)
        {
            auto new_father = std::make_shared<InternalNode>();
            new_father->children[now->children.rbegin()->first] = now;
            new_father->children[new_internal->children.rbegin()->first] = new_internal;
            now->parent = new_father;
            new_internal->parent = new_father;
            new_father->map = now->map | new_internal->map;
            return new_father;
        }
        else
        {
            auto old_father = std::dynamic_pointer_cast<InternalNode>(father);
            old_father->children[now->children.rbegin()->first] = now;
            old_father->children[new_internal->children.rbegin()->first] = new_internal;
            if (old_father->children.size() > max_children)
                return split(old_father, seed);
            update_map(old_father);
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
inline bool IndexTree::in_leaf(std::shared_ptr<LeafNode> leaf, const std::string &key) const
{
    return leaf->datas.find(key) != leaf->datas.end();
}

/**
 * Inserts a new key-value pair into the index tree.
 * @param tree A shared pointer to the tree to insert into.
 * @param key The key to insert.
 * @param data The data to associate with the key.
 */
void IndexTree::insert(std::shared_ptr<Tree> tree, const std::string &key, const std::vector<std::string> &data)
{
    if (tree == nullptr)
        return;
    auto now = find_leaf(tree, key);
    #ifdef DEBUG
        assert(now != nullptr);
    #endif
    if (in_leaf(now, key))
        return;
    if (!tree->root->is_leaf && key > now->datas.rbegin()->first)
    {
        auto internal = std::dynamic_pointer_cast<InternalNode>(tree->root);
        while (true)
        {
            auto last = internal->children.rbegin();
            auto child = last->second;
            internal->children.erase((++last).base());
            internal->children[key] = child;
            if (child->is_leaf)
                break;
            internal = std::dynamic_pointer_cast<InternalNode>(child);
        }
    }
    now->datas[key] = data;
    tree->cnt++;
    merge_map(now, tree->seed);
    if (now->datas.size() == max_children)
    {
        auto old_root = tree->root;
        auto new_parent = split(now, tree->seed);
        if (old_root->parent != nullptr)
            tree->root = new_parent;
        return;
    }
    update_map(now->parent);
    return;
}

/**
 * Finds the leaf node in the given tree that contains the specified key.
 * @param tree The tree to search.
 * @param key The key to search for.
 * @return A shared pointer to the leaf node containing the key, or nullptr if the key is not found.
 */
std::shared_ptr<IndexTree::LeafNode> IndexTree::find_leaf(std::shared_ptr<Tree> tree, const std::string &key)
{
    if (tree->root->is_leaf)
        return std::dynamic_pointer_cast<LeafNode>(tree->root);
    auto now = std::dynamic_pointer_cast<InternalNode>(tree->root);
    while (true)
    {
        auto child = now->children.upper_bound(key);
        if (child == now->children.end())
            child--;
        if (child != now->children.begin())
        {
            auto prev = std::prev(child);
            if (prev->first >= key)
                child--;
        }
        auto next = child->second;
        if (next->is_leaf)
            return std::dynamic_pointer_cast<LeafNode>(next);
        now = std::dynamic_pointer_cast<InternalNode>(next);
    }
    return nullptr;
}

/**
 * This function performs a fuzzy search for a given key in the IndexTree.
 * It returns a vector of shared pointers to the LeafNodes that contain the key.
 * @param tree A shared pointer to the tree to search in.
 * @param key The key to search for.
 * @return A vector of shared pointers to the LeafNodes that contain the key.
 */
std::vector<std::shared_ptr<IndexTree::LeafNode>> IndexTree::fuzzy_find_leaf(std::shared_ptr<Tree> tree, const std::string &key)
{
    auto hash = this->hash_table.get_index(key, tree->seed);
    auto is_all_inmap = [&hash](const std::bitset<HashTable::PRIME> &map) -> bool
    {
        for (const auto &pos : hash)
            if (!map[pos])
                return false;
        return true;
    };
    std::vector<std::shared_ptr<LeafNode>> ret;

    if (tree->root->is_leaf)
    {
        auto tmp = std::dynamic_pointer_cast<LeafNode>(tree->root);
        if (is_all_inmap(tmp->map))
            ret.push_back(tmp);
        return ret;
    }
    auto tmp = std::dynamic_pointer_cast<InternalNode>(tree->root);
    if (!is_all_inmap(tmp->map))
        return ret;
    std::queue<std::shared_ptr<InternalNode>> internal_nodes_queue;
    internal_nodes_queue.push(std::move(tmp));
    while (!internal_nodes_queue.empty())
    {
        auto now = std::move(internal_nodes_queue.front());
        internal_nodes_queue.pop();
        for (const auto &[key, child] : now->children)
        {
            if (child->is_leaf)
            {
                auto leaf = std::dynamic_pointer_cast<LeafNode>(child);
                if (is_all_inmap(leaf->map))
                    ret.push_back(std::move(leaf));
            }
            else
            {
                auto internal = std::dynamic_pointer_cast<InternalNode>(child);
                if (is_all_inmap(internal->map))
                    internal_nodes_queue.push(std::move(internal));
            }
        }
    }
    return ret;
}

/**
 * Merges a node with its sibling if it has less than max_children/2 children.
 * If the node is a leaf node, it merges with its sibling leaf node.
 * If the node is an internal node, it merges with its sibling internal node.
 * If the node's parent has less than max_children/2 children after the merge, the parent is merged with its sibling.
 * @param node The node to be merged.
 * @param seed The seed used for random number generation.
 */
void IndexTree::merge(std::shared_ptr<Node> node, const unsigned long long &seed)
{
    auto father = node->parent;
    if (father == nullptr)
    {
        if (!node->is_leaf)
        {
            auto now = std::dynamic_pointer_cast<InternalNode>(node);
            if (now->children.size() == 1)
            {
                tree->root = now->children.begin()->second;
                tree->root->parent = nullptr;
            }
        }
        return;
    }
    if (father->children.size() == 1)
    {
        return merge(father, seed);
    }
    if (node->is_leaf)
    {
        auto now = std::dynamic_pointer_cast<LeafNode>(node);
        auto get_brother = [](std::shared_ptr<LeafNode> now) -> std::shared_ptr<LeafNode>
        {
            auto father = now->parent;
            auto tmp = father->children.find(now->datas.rbegin()->first);
            if (tmp == father->children.begin())
                return nullptr;
            auto it = std::prev(tmp);
            return std::dynamic_pointer_cast<LeafNode>(it->second);
        };

        auto brother = get_brother(now);
        if (brother == nullptr)
        {
            auto it = father->children.find(now->datas.rbegin()->first);
            if (it == father->children.begin())
            {
                auto cousin = std::dynamic_pointer_cast<LeafNode>(now->next);
                cousin->datas.merge(now->datas);
                // TODO: FileOperation
                father->children.erase(it);
                merge_map(cousin, seed);
                merge_map(father);
                if (father->children.size() < max_children / 2)
                    return merge(father, seed);
                update_map(father->parent);
            }
            return;
        }
        if (brother->datas.size() - 1 >= max_children / 2)
        {
            auto max = brother->datas.rbegin();
            father->children.erase(max->first);
            auto max_key = max->first;
            now->datas.insert(std::move(*max));
            brother->datas.erase(max_key);
            father->children[brother->datas.rbegin()->first] = brother;
            merge_map(now, seed);
            merge_map(brother, seed);
            update_map(father);
            return;
        }
        auto cousin = get_brother(brother);
        if (cousin != nullptr)
            cousin->next = now;
        auto max = brother->datas.rbegin();
        father->children.erase(max->first);
        now->datas.merge(brother->datas);
        // TODO: FileOperation
        merge_map(now, seed);
        merge_map(father);
        if (father->children.size() < max_children / 2)
            return merge(father, seed);
        update_map(father->parent);
    }
    else
    {
        auto now = std::dynamic_pointer_cast<InternalNode>(node);
        auto get_brother = [](std::shared_ptr<InternalNode> now) -> std::shared_ptr<InternalNode>
        {
            auto father = now->parent;
            auto tmp = father->children.find(now->children.rbegin()->first);
            if (tmp == father->children.begin())
                return nullptr;
            auto it = std::prev(tmp);
            return std::dynamic_pointer_cast<InternalNode>(it->second);
        };

        auto brother = get_brother(now);
        if (brother == nullptr)
        {
            auto father = now->parent;
            auto it = father->children.find(now->children.rbegin()->first);
            if (it == father->children.begin())
            {
                auto cousin = std::dynamic_pointer_cast<InternalNode>((++it)->second);
                auto max = now->children.rbegin()->first;
                for (auto &[key, child] : now->children)
                    child->parent = cousin;
                cousin->children.merge(now->children);
                father->children.erase(max);
                merge_map(cousin);
                merge_map(father);
                if (father->children.size() < max_children / 2)
                    return merge(father, seed);
                update_map(father->parent);
            }
            return;
        }
        if (brother->children.size() - 1 >= max_children / 2)
        {
            auto max = brother->children.rbegin();
            auto max_key = max->first;
            father->children.erase(max_key);
            max->second->parent = now;
            now->children.insert(std::move(*max));
            brother->children.erase(max_key);
            father->children[brother->children.rbegin()->first] = brother;
            merge_map(now);
            merge_map(brother);
            update_map(father);
            return;
        }
        auto max = brother->children.rbegin();
        father->children.erase(max->first);
        for (const auto &[key, child] : brother->children)
            child->parent = now;
        now->children.merge(brother->children);
        merge_map(now);
        merge_map(father);
        if (father->children.size() < max_children / 2)
            return merge(father, seed);
        update_map(father->parent);
    }
}

/**
 * Removes a key from the index tree.
 * @param tree A shared pointer to the tree.
 * @param key The key to be removed.
 */
void IndexTree::remove(std::shared_ptr<Tree> tree, const std::string &key)
{
    auto now = find_leaf(tree, key);
    #ifdef DEBUG
        assert(now != nullptr);
    #endif
    if (!in_leaf(now, key))
        return;
    tree->cnt--;
    if (now->datas.size() == 1)
    {
        if (now == tree->root)
        {
            tree->root = nullptr;
            // TODO: FileOperation
            return;
        }
        auto father = now->parent;
        father->children.erase(key);
        // TODO: FileOperation
        merge_map(father);
        if (father->children.size() < max_children / 2)
            return merge(father, tree->seed);
        update_map(father->parent);
        return;
    }
    auto max = now->datas.rbegin()->first;
    now->datas.erase(key);
    if (key == max && now->parent != nullptr)
    {
        auto internal = std::dynamic_pointer_cast<InternalNode>(now->parent);
        if (internal->children.rbegin()->first != key)
        {
            internal->children.erase(key);
            internal->children[now->datas.rbegin()->first] = now;
        }
        else
        {
            internal->children.erase(key);
            internal->children[now->datas.rbegin()->first] = now;
            auto modify = internal;
            while (modify->parent != nullptr)
            {
                auto father = std::dynamic_pointer_cast<InternalNode>(modify->parent);
                auto max = father->children.rbegin()->first;
                father->children.erase(key);
                father->children[modify->children.rbegin()->first] = modify;
                if (key == max)
                    modify = father;
                else
                    break;
            }
        }
    }
    merge_map(now, tree->seed);
    update_map(now->parent);
    // TODO: FileOperation
    if (now->datas.size() < max_children / 2)
    {
        merge(now, tree->seed);
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
    return;
}