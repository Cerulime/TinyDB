#include <iostream>
#include <cassert>
#include "../include/HashTable.hpp"

void test_hash_table() {
    // Test create_map and find
    std::string s1 = "hello";
    std::string s2 = "world";
    std::bitset<HashTable::PRIME> map = HashTable::create_map(s1 + s2, 12345);
    assert(HashTable::find(map, 12345, s1));
    assert(HashTable::find(map, 12345, s2));
    assert(!HashTable::find(map, 12345, "foo"));

    // Test get_hash
    const auto &[lw1, hg1] = HashTable::get_hash(s1.data(), s1.length(), 12345);
    const auto &[lw2, hg2] = HashTable::get_hash(s2.data(), s2.length(), 12345);
    assert(lw1 != lw2 || hg1 != hg2);
}

int main() {
    test_hash_table();
    std::cout << "All tests passed!\n";
    return 0;
}