#include "Utils.hpp"

#include "../namespace.hpp"

#include "./HexUtils.hpp"

string Utils::binaryToString(const vector<uint8_t> &bytes)
{
  return HexUtils::binaryToString(bytes);
}

string Utils::binaryToHex(const vector<uint8_t> &bytes)
{
  return HexUtils::binaryToHex(bytes);
}

binary_t Utils::binaryConcat(const binary_t &a, const binary_t &b)
{
  return HexUtils::concat(a, b);
}

binary_t Utils::binaryPadZero(const binary_t &bytes, int length)
{
  return HexUtils::padZero(bytes, length);
}

vector<uint8_t> Utils::stringToBinary(const string &plainText)
{
  return HexUtils::stringToBinary(plainText);
}

string Utils::stringToHex(const string &plainText)
{
  return HexUtils::encode(plainText);
}

string Utils::hexToString(const string &plainText)
{
  return HexUtils::decode(plainText);
}

vector<uint8_t> Utils::hexToBinary(const string &hexString)
{
  string plainText = HexUtils::decode(hexString);
  return HexUtils::stringToBinary(plainText);
}
