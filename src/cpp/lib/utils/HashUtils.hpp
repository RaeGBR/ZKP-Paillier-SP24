#pragma once

#include <string>

#include "sha.h"
#include "hex.h"

#include "./HexUtils.hpp"

using namespace std;
using CryptoPP::byte;
using CryptoPP::SHA256;

namespace cryptoplus
{

class HashUtils
{
private:
  static SHA256 sha256;

public:
  static string calcHash(string value, bool decodeInput = false, bool encodeOutput = true);
  static vector<uint8_t> calcHash(vector<uint8_t> value);
};

} // namespace cryptoplus
