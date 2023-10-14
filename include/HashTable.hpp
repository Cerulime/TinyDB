#pragma once
#include "Include.hpp"

/*
n = 500
p = 1e-4
m = 9587
k = 13
*/
class HashTable
{
private:
    static const int MAX_STR_LEN = 10;
    unsigned long long fmix(unsigned long long);
    std::tuple<unsigned long long, unsigned long long> get_hash(const void *, const unsigned int &, const unsigned int &);
public:
    static const int K_HASH = 13;
    static const unsigned int PRIME = 9587;
    HashTable();
    std::mt19937_64 get_rand;
    bool find(const std::bitset<PRIME> &, const unsigned long long &, const std::string &);
    std::bitset<PRIME> create_map(const std::string_view &, const unsigned long long &);
    std::vector<int> get_index(const std::string &, const unsigned long long &);
};