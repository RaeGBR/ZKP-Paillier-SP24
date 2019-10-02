#include "gtest/gtest.h"

#include "HashUtils.hpp"
#include "Utils.hpp"

#include "sha.h"
#include "hex.h"

using namespace std;
using namespace cryptoplus;

namespace
{

const auto message = Utils::stringToBinary("MESSAGE");
const string answer = "B194D92018D6074234280C5F5B88649C8DB14EF4F2C3746D8A23896A0F6F3B66";

TEST(HashUtils, SHA256)
{
  auto result = HashUtils::sha256(message);
  EXPECT_EQ(Utils::binaryToHex(result), answer);

  auto result2 = HashUtils::sha256(message);
  EXPECT_EQ(Utils::binaryToHex(result2), answer);
  EXPECT_EQ(Utils::binaryToHex(result2), Utils::binaryToHex(result));

  const auto message2 = Utils::stringToBinary("MESSAGE2");
  const string answer3 = "ECBB828A6F634EED1E3E43EBB4378113A0832CD8CCAC8B0A74B511452FC8A35A";
  auto result3 = HashUtils::sha256(message2);
  EXPECT_NE(Utils::binaryToHex(result3), Utils::binaryToHex(result2));
  EXPECT_EQ(Utils::binaryToHex(result3), answer3);
}

TEST(HashUtils, SHA256_binary_input)
{
  const string messageHex = "4D455353414745";
  const vector<uint8_t> binary(messageHex.begin(), messageHex.end());
  vector<uint8_t> result = HashUtils::sha256(binary);
  CryptoPP::byte *b = &result[0];
  string encoded;
  CryptoPP::StringSource ss(b, CryptoPP::SHA256::DIGESTSIZE, true, new CryptoPP::HexEncoder(new CryptoPP::StringSink(encoded)));
  EXPECT_EQ(encoded, "77089D0E5B1C6CE64A1628A0C4731EF3866C8DC7FF507E106E6145EE57EBC02D");
  EXPECT_EQ(Utils::binaryToHex(HashUtils::sha256(Utils::stringToBinary("4D455353414745"))), "77089D0E5B1C6CE64A1628A0C4731EF3866C8DC7FF507E106E6145EE57EBC02D");
}

} // namespace
