#include "Include.hpp"

/*
n = 971
p = 1e-4
m = 18615
k = 13
*/

namespace HashTable
{
    static const int str_len = 50;
    static const int k_hash = 13;
    static const unsigned int prime = 18617;
    std::mt19937_64 get_rand(std::random_device{}());

    unsigned long long fmix(unsigned long long);
    std::tuple<unsigned long long, unsigned long long> get_hash(const void *, const unsigned int &, const unsigned int &);
    std::bitset<prime> create_map(const std::string &, unsigned long long);
    bool find(const std::bitset<prime> &, const unsigned long long &, const std::string &);
}