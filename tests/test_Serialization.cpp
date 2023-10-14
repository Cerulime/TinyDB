#include "..\include\Serialization.hpp"
#include <gtest/gtest.h>

TEST(SerializationTest, Base64)
{
    std::string str = "Hello World!";
    std::string encoded = Serialization::base64encode(str.c_str(), str.size());
    EXPECT_EQ(encoded, "SGVsbG8gV29ybGQh");
    char decoded[20] = {};
    Serialization::base64decode(encoded, decoded, 20);
    EXPECT_EQ(str, decoded);
}

TEST(Serialization, Map)
{
    std::bitset<HashTable::PRIME> map;
    map.set(1);
    map.set(3);
    map.set(5);
    std::string encoded = Serialization::serialize_map(map);
    std::bitset<HashTable::PRIME> decoded = Serialization::deserialize_map(encoded);
    decoded.set(1);
    decoded.set(3);
    decoded.set(5);
    EXPECT_EQ(map, decoded);
}