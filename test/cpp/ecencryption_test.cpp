#include "gtest/gtest.h"

#include "aes.h"

#include "Utils.hpp"

#include "lib/utils/AESEncryption.hpp"
namespace
{
string s1 = "foo bar baz";
TEST(AESEncryption, encryptAndDecrypt)
{

  const string k1 = "123456789";
  const string i1 = "123456789";

  string cypherText = AESEncryption::aesEncrypt(k1, i1, s1);
  string decypherText = AESEncryption::aesDecrypt(k1, i1, cypherText);
  ASSERT_EQ(decypherText, s1);

  const string k2 = "987654321";
  const string i2 = "987654321";
  cypherText = AESEncryption::aesEncrypt(k2, i2, s1);
  ASSERT_THROW(AESEncryption::aesDecrypt(k1, i2, cypherText), std::exception);
}

TEST(AESEncryption, longEncryption)
{
  const string k1 = "1234567890123456abcdef";
  const string i1 = "1234567890123456abcdef";

  ASSERT_THROW(AESEncryption::aesEncrypt(k1, i1, s1), std::invalid_argument);
}

TEST(AESEncryption, encryptAndDecryptThroughUtils)
{
  const vector<uint8_t> k1 = Utils::random(CryptoPP::AES::DEFAULT_KEYLENGTH);
  const vector<uint8_t> i1 = Utils::random(CryptoPP::AES::DEFAULT_KEYLENGTH);
  const vector<uint8_t> s1bin = Utils::stringToBinary(s1);

  vector<uint8_t> cypherText = Utils::aesEncrypt(k1, i1, s1bin);
  vector<uint8_t> decypherText = Utils::aesDecrypt(k1, i1, cypherText);
  ASSERT_EQ(decypherText, s1bin);

  const vector<uint8_t> k2 = Utils::hexToBinary("7632b66a3e3eaa3327d34fe0370f2ad5");
  const vector<uint8_t> i2 = Utils::hexToBinary("7632b66a3e3eaa3327d34fe0370f2ad5");
  const vector<uint8_t> ans = Utils::hexToBinary("7172080E0D45407D8E239B9BECC85B76");
  cypherText = Utils::aesEncrypt(k2, i2, s1bin);
  decypherText = Utils::aesDecrypt(k2, i2, cypherText);
  ASSERT_EQ(cypherText, ans);
  ASSERT_EQ(decypherText, s1bin);
}
} // namespace
