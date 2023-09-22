#include "..\.\include\Serialization.hpp"

std::string Serialization::base64encode(const char *bytes, const std::size_t &size)
{
    std::string result;
    result.reserve((size + 2) / 3 * 4);
    for (std::size_t i = 0; i < size; i += 3)
    {
        int val = (bytes[i] << 16) + (i + 1 < size ? bytes[i + 1] << 8 : 0) + (i + 2 < size ? bytes[i + 2] : 0);
        result.push_back(Serialization::base64_table[(val >> 18) & 0x3F]);
        result.push_back(Serialization::base64_table[(val >> 12) & 0x3F]);
        if (i + 1 < size)
            result.push_back(Serialization::base64_table[(val >> 6) & 0x3F]);
        if (i + 2 < size)
            result.push_back(Serialization::base64_table[val & 0x3F]);
    }
    while (result.size() % 4)
        result.push_back('=');
    return result;
}

void Serialization::base64decode(const std::string &str, char* bytes, const std::size_t &size)
{
    std::size_t len = str.size();
    std::size_t i = 0, j = 0;
    while (i < len && j < size)
    {
        int val = 0;
        for (int k = 0; k < 4 && i < len; i++, k++)
        {
            val <<= 6;
            if (str[i] >= 'A' && str[i] <= 'Z')
                val |= str[i] - 'A';
            else if (str[i] >= 'a' && str[i] <= 'z')
                val |= str[i] - 'a' + 26;
            else if (str[i] >= '0' && str[i] <= '9')
                val |= str[i] - '0' + 52;
            else if (str[i] == '+')
                val |= 62;
            else if (str[i] == '/')
                val |= 63;
            else if (str[i] == '=')
                val |= 0;
            else
                throw std::runtime_error("Invalid base64 string");
        }
        bytes[j++] = (val >> 16) & 0xFF;
        if (j < size)
            bytes[j++] = (val >> 8) & 0xFF;
        if (j < size)
            bytes[j++] = val & 0xFF;
    }
}

/**
 * Serializes a bitset map into a base64-encoded string.
 * @param map The bitset map to be serialized.
 * @return The base64-encoded string representation of the serialized map.
 */
std::string Serialization::serialize_map(const std::bitset<HashTable::prime> &map)
{
    constexpr std::size_t size = (HashTable::prime + 7) / 8;
    char bytes[size] = {0};
    for (std::size_t i = 0; i < HashTable::prime; i++)
        bytes[i / 8] |= (map[i] << (i % 8));
    return Serialization::base64encode(bytes, size);
}
/**
 * @brief Deserializes a string into a bitset representing a hash table.
 * 
 * @param str The string to deserialize.
 * @return std::bitset<HashTable::prime> The deserialized bitset.
 * @throws std::runtime_error If there is an error during deserialization.
 */
std::bitset<HashTable::prime> Serialization::deserialize_map(const std::string &str)
{
    constexpr std::size_t size = (HashTable::prime + 7) / 8;
    char bytes[size] = {0};
    try
    {
        Serialization::base64decode(str, bytes, size);
    }
    catch (const std::runtime_error &e)
    {
        throw e;
    }
    std::bitset<HashTable::prime> map;
    for (std::size_t i = 0; i < HashTable::prime; i++)
        map[i] = (bytes[i / 8] >> (i % 8)) & 1;
    return map;
}