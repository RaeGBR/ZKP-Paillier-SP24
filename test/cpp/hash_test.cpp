#include "gtest/gtest.h"

#include "lib/utils/HashUtils.hpp"

using namespace std;
using namespace cryptoplus;

namespace
{

const string message = "MESSAGE";
const string answer = "B194D92018D6074234280C5F5B88649C8DB14EF4F2C3746D8A23896A0F6F3B66";

TEST(HashUtils, SHA256)
{
  string result = HashUtils::calcHash(message);
  stringstream ss;
  ss << std::hex << result;
  EXPECT_EQ(ss.str(), answer);

  string result2 = HashUtils::calcHash(message);
  stringstream ss2;
  ss2 << std::hex << result2;
  EXPECT_EQ(ss2.str(), answer);
  EXPECT_EQ(ss2.str(), ss.str());

  const string message2 = "MESSAGE2";
  const string answer3 = "ECBB828A6F634EED1E3E43EBB4378113A0832CD8CCAC8B0A74B511452FC8A35A";
  string result3 = HashUtils::calcHash(message2);
  stringstream ss3;
  ss3 << std::hex << result3;
  EXPECT_NE(ss3.str(), ss2.str());
  EXPECT_EQ(ss3.str(), answer3);
}

TEST(HashUtils, SHA256_raw_input_output)
{
  const string messageHex1 = "4D455353414745";
  const string messageHex2 = "4D4553534147450000000000000000000000000000000000000000000000000000000000000000";
  const string message2 = HexUtils::decode(messageHex2);

  string h1 = HashUtils::calcHash(message, false, true);
  string h2 = HashUtils::calcHash(messageHex1, true, true);
  string h3 = HashUtils::calcHash(messageHex2, true, true);
  string h4 = HashUtils::calcHash(message2, false, true);
  string h5 = HashUtils::calcHash(message2 + message2, false, true);
  EXPECT_EQ(h1, h2);
  EXPECT_NE(h1, h3);
  EXPECT_EQ(h3, "4AD08AA80763996283E2949487D871C49EDCF4DE919B7BA4CA3F1AADA67EF79C");
  EXPECT_EQ(h3, h4);
  EXPECT_EQ(h5, "1AE16000186B0106E3468706D7A89FC630AA710FFBBB0438BD3BE1932E44A7DC");
}

TEST(HashUtils, SHA256_binary_input)
{
  const string messageHex = "4D455353414745";
  const vector<uint8_t> binary(messageHex.begin(), messageHex.end());
  vector<uint8_t> result = HashUtils::calcHash(binary);
  CryptoPP::byte *b = &result[0];
  string encoded;
  CryptoPP::StringSource ss(b, CryptoPP::SHA256::DIGESTSIZE, true, new CryptoPP::HexEncoder(new CryptoPP::StringSink(encoded)));
  EXPECT_EQ(encoded, "77089D0E5B1C6CE64A1628A0C4731EF3866C8DC7FF507E106E6145EE57EBC02D");
  EXPECT_EQ(HashUtils::calcHash("4D455353414745"), "77089D0E5B1C6CE64A1628A0C4731EF3866C8DC7FF507E106E6145EE57EBC02D");
}

} // namespace
