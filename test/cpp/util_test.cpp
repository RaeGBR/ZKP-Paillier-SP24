#include "gtest/gtest.h"

#include "Utils.hpp"


using namespace std;
using namespace cryptoplus;


namespace{
    TEST(Utils, binaryToString)
    {
        vector<uint8_t> binary = {0x66, 0x6f, 0x6f, 0x62, 0x61, 0x72, 0x62, 0x61, 0x7a};
        EXPECT_EQ(Utils::binaryToString(binary), "foobarbaz");
    }

    TEST(Utils, binaryToHex)
    {
        vector<uint8_t> binary = {0x66, 0x6f, 0x6f, 0x62, 0x61, 0x72, 0x62, 0x61, 0x7a};
        EXPECT_EQ(Utils::binaryToHex(binary), "666F6F62617262617A");
    }

    TEST(Utils, stringToBinary)
    {
        string text = "foobarbaz";
        vector<uint8_t> expectedResult = {0x66, 0x6f, 0x6f, 0x62, 0x61, 0x72, 0x62, 0x61, 0x7a};
        EXPECT_EQ(Utils::stringToBinary(text), expectedResult);
    }

    TEST(Utils, hextToBinary)
    {
        string text = "foobarbaz";
        string hexText = Utils::stringToHex(text);
        vector<uint8_t> hexToBinary = Utils::hexToBinary(hexText);
        vector<uint8_t> stringToBinary = Utils::stringToBinary(text);

        EXPECT_EQ(hexToBinary, stringToBinary);
    }
}
