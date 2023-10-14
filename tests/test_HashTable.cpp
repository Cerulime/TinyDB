#include "..\include\HashTable.hpp"
#include <gtest/gtest.h>

TEST(HashTableTest, CreateMap)
{
    HashTable ht;
    std::string_view s = "apple,banana,orange,";
    unsigned long long seed = 123456789;
    std::bitset<HashTable::PRIME> map = ht.create_map(s, seed);
    std::vector<int> index = ht.get_index("ban", seed);
    const int K_HASH = HashTable::K_HASH;
    EXPECT_EQ(index.size(), K_HASH);
    for (auto &x : index)
        EXPECT_TRUE(map.test(x));
}

TEST(HashTableTest, CreateMapEmptyString)
{
    HashTable ht;
    std::string_view s = "";
    unsigned long long seed = 123456789;
    std::bitset<HashTable::PRIME> map = ht.create_map(s, seed);
    EXPECT_FALSE(map.any());
}

TEST(HashTableTest, CreateMapLongString)
{
    HashTable ht;
    std::string_view s = "apple,banana,orange,grapefruit,watermelon,pineapple,kiwi,pear,peach,plum,";
    unsigned long long seed = 987654321;
    std::bitset<HashTable::PRIME> map = ht.create_map(s, seed);
    std::vector<int> index = ht.get_index("apple", seed);
    const int K_HASH = HashTable::K_HASH;
    EXPECT_EQ(index.size(), K_HASH);
    for (auto &x : index)
        EXPECT_TRUE(map.test(x));
}

TEST(HashTableTest, CreateMapSingleString)
{
    HashTable ht;
    std::string_view s = "apple,";
    unsigned long long seed = 123456789;
    std::bitset<HashTable::PRIME> map = ht.create_map(s, seed);
    std::vector<int> index = ht.get_index("app", seed);
    const int K_HASH = HashTable::K_HASH;
    EXPECT_EQ(index.size(), K_HASH);
    for (auto &x : index)
        EXPECT_TRUE(map.test(x));
}