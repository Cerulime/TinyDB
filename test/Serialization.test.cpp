#include <iostream>
#include <cassert>
#include "../include/Serialization.hpp"

int main() {
    // Test base64encode
    std::string input1 = "Hello, world!";
    std::string expected_output1 = "SGVsbG8sIHdvcmxkIQ==";
    std::string output1 = Serialization::base64encode(input1.c_str(), input1.size());
    assert(output1 == expected_output1);

    std::string input2 = "This is a test.";
    std::string expected_output2 = "VGhpcyBpcyBhIHRlc3Qu";
    std::string output2 = Serialization::base64encode(input2.c_str(), input2.size());
    assert(output2 == expected_output2);

    // Test base64decode
    std::string input3 = "SGVsbG8sIHdvcmxkIQ==";
    std::string expected_output3 = "Hello, world!";
    char output3[expected_output3.size() + 1];
    Serialization::base64decode(input3, output3, expected_output3.size());
    output3[expected_output3.size()] = '\0';
    assert(expected_output3 == output3);

    std::string input4 = "VGhpcyBpcyBhIHRlc3Qu";
    std::string expected_output4 = "This is a test.";
    char output4[expected_output4.size() + 1];
    Serialization::base64decode(input4, output4, expected_output4.size());
    output4[expected_output4.size()] = '\0';
    assert(expected_output4 == output4);

    // Test serialize_map and deserialize_map
    std::bitset<HashTable::prime> input5;
    input5[0] = 1;
    input5[1] = 0;
    input5[2] = 1;
    input5[3] = 1;
    input5[4] = 0;
    input5[5] = 1;
    input5[6] = 0;
    input5[7] = 1;
    std::string expected_output5 = "JQ==";
    std::string output5 = Serialization::serialize_map(input5);
    assert(output5 == expected_output5);

    std::bitset<HashTable::prime> expected_output6;
    expected_output6[0] = 1;
    expected_output6[1] = 0;
    expected_output6[2] = 1;
    expected_output6[3] = 1;
    expected_output6[4] = 0;
    expected_output6[5] = 1;
    expected_output6[6] = 0;
    expected_output6[7] = 1;
    std::bitset<HashTable::prime> output6 = Serialization::deserialize_map(expected_output5);
    assert(output6 == expected_output6);

    std::cout << "All tests passed!\n";
    return 0;
}