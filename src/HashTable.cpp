#include "..\.\include\HashTable.hpp"

unsigned long long HashTable::fmix(unsigned long long x)
{
    x ^= x >> 33;
    x *= 0xff51afd7ed558ccdull;
    x ^= x >> 33;
    x *= 0xc4ceb9fe1a85ec53ull;
    x ^= x >> 33;
    return x;
}

std::tuple<unsigned long long, unsigned long long> HashTable::get_hash(const void *key, const unsigned int &len, const unsigned int &seed)
{
    const unsigned char *data = (const unsigned char *)key;
    const unsigned long long *blocks = (const unsigned long long *)data;
    const int n = len / 16;
    const unsigned long long c1 = 0x87c37b91114253d5ull, c2 = 0x4cf5ad432745937full;
    unsigned long long lw = seed, hg = seed;

    for (int i = 0; i < n; i++)
    {
        unsigned long long k1 = blocks[i<<1];
        unsigned long long k2 = blocks[i<<1|1];
        k1 *= c1, k1 = _rotl64(k1, 31), k1 *= c2, lw ^= k1;
        lw = _rotl64(lw, 27), lw += hg, lw = lw*5 + 0x52dce729;
        k2 *= c2, k2 = _rotl64(k2, 33), k2 *= c1, hg ^= k2;
        hg = _rotl64(hg, 31), hg += lw, hg = hg*5 + 0x38495ab5;
    }

    const unsigned char *tail = (const unsigned char *)(data + n*16);
    unsigned long long k1 = 0, k2 = 0;
    switch (len & 15)
    {
        case 15: k2 ^= ((unsigned long long)tail[14]) << 48;
        case 14: k2 ^= ((unsigned long long)tail[13]) << 40;
        case 13: k2 ^= ((unsigned long long)tail[12]) << 32;
        case 12: k2 ^= ((unsigned long long)tail[11]) << 24;
        case 11: k2 ^= ((unsigned long long)tail[10]) << 16;
        case 10: k2 ^= ((unsigned long long)tail[ 9]) << 8;
        case  9: k2 ^= ((unsigned long long)tail[ 8]);
                 k2 *= c2, k2 = _rotl64(k2, 33), k2 *= c1, hg ^= k2;
        
        case  8: k1 ^= ((unsigned long long)tail[ 7]) << 56;
        case  7: k1 ^= ((unsigned long long)tail[ 6]) << 48;
        case  6: k1 ^= ((unsigned long long)tail[ 5]) << 40;
        case  5: k1 ^= ((unsigned long long)tail[ 4]) << 32;
        case  4: k1 ^= ((unsigned long long)tail[ 3]) << 24;
        case  3: k1 ^= ((unsigned long long)tail[ 2]) << 16;
        case  2: k1 ^= ((unsigned long long)tail[ 1]) << 8;
        case  1: k1 ^= ((unsigned long long)tail[ 0]);
                 k1 *= c1, k1 = _rotl64(k1, 31), k1 *= c2, lw ^= k1;
    }

    lw ^= len, hg ^= len;
    lw += hg, hg += lw;
    lw = fmix(lw), hg = fmix(hg);
    lw += hg, hg += lw;

    return std::make_tuple(lw, hg);
}

std::bitset<HashTable::prime> HashTable::create_map(const std::string &s, unsigned long long seed)
{
    std::bitset<18617> map;
    map.reset();

    int len = s.length();
    for (int i = 0; i < len; i++)
    {
        char ch[HashTable::str_len] = {};
        for (int j = 0; i + j < len; j++)
        {
            ch[j] = s[i + j];
            const auto &[lw, hg] = HashTable::get_hash(ch, j+1, seed);
            for (int k = 1; k <= HashTable::k_hash; k++)
                map[(lw + hg*k) % HashTable::prime]=1;
        }
    }
    return map;
}

bool HashTable::find(const std::bitset<HashTable::prime> &map, const unsigned long long &seed, const std::string &s)
{
    const auto &[lw, hg] = HashTable::get_hash(s.data(), s.length(), seed);
    for (int k = 1; k <= HashTable::k_hash; k++)
        if (!map[(lw + hg*k) % HashTable::prime])
            return false;
    return true;
}