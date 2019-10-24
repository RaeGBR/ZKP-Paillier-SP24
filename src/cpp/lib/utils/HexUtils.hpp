#pragma once

#include "../namespace.hpp"

#include <numeric>
#include <iostream>
#include <sstream>

#include "hex.h"

using namespace std;

namespace cryptoplus
{

class HexUtils
{
public:
  static string encode(const string &data);
  static string decode(const string &data);
  static CryptoPP::HexEncoder *getHexEncoder(string &data);
  static CryptoPP::HexDecoder *getHexDecoder(string &data);
  static CryptoPP::StringSink *getStringSink(string &data);
  static string binaryToString(const binary_t &binary);
  static string binaryToHex(const binary_t &binary);
  static json binaryToJson(const binary_t &binary);
  static json binaryToJson(const vector<binary_t> &binary);
  static binary_t stringToBinary(const string &plainText);
  static binary_t concat(const binary_t &a, const binary_t &b);
  static binary_t concat(const binary_t &a, const string &b);
  static binary_t concat(const string &a, const binary_t &b);
  static void append(binary_t &a, const binary_t &b);
  static binary_t padZero(const binary_t &data, const size_t length);
  static bool eq(const binary_t &a, const binary_t &b);
};

} // namespace cryptoplus

using namespace cryptoplus;
