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
public:
    static const int K_HASH = 13;
    static const unsigned int PRIME = 9587;

private:
    static const int MAX_STR_LEN = 10;
    std::unordered_map<std::string_view, std::bitset<PRIME>> cache;
    unsigned long long fmix(unsigned long long);
    std::tuple<unsigned long long, unsigned long long> get_hash(const void *, const unsigned int &, const unsigned int &);

public:
    HashTable();
    std::mt19937_64 get_rand;
    bool find(const std::bitset<PRIME> &, const unsigned long long &, const std::string &);
    std::bitset<PRIME> create_map(const std::string_view &, const unsigned long long &);
    std::vector<int> get_index(const std::string &, const unsigned long long &);
};