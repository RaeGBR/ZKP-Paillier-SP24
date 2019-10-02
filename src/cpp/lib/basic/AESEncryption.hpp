#pragma once

#include <fstream>
#include <string>
#include <iostream>
#include <iomanip>

#include "eccrypto.h"
#include "osrng.h"
#include "oids.h"
#include "hex.h"
#include "aes.h"
#include "modes.h"
#include "filters.h"

#include "ECCurve.hpp"

#include "../utils/HexUtils.hpp"

using namespace std;
using namespace cryptoplus;

namespace cryptoplus
{

class AESEncryption
{
public:
  static string aesEncrypt(const string &key, const string &iv, const string &plainText);
  static string aesDecrypt(const string &key, const string &iv, const string &encryptedText);
};

} // namespace cryptoplus
