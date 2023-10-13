#pragma once
#include "Include.hpp"

/*
n = 500
p = 1e-4
m = 9587
k = 13
*/

namespace HashTable
{
    inline const int MAX_STR_LEN = 10;
    inline const int K_HASH = 13;
    inline const unsigned int PRIME = 9587;
    extern std::mt19937_64 get_rand;

    unsigned long long fmix(unsigned long long);
    std::tuple<unsigned long long, unsigned long long> get_hash(const void *, const unsigned int &, const unsigned int &);
    std::bitset<PRIME> create_map(const std::string_view &, const unsigned long long &);
    std::vector<int> get_index(const std::string &, const unsigned long long &);
    bool find(const std::bitset<PRIME> &, const unsigned long long &, const std::string &);
}