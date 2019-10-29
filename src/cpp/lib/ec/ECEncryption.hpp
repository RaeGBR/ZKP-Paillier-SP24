#pragma once

#include <fstream>
#include <string>
#include <iostream>
#include <iomanip>

#include "eccrypto.h"
#include "osrng.h"
#include "oids.h"
#include "randpool.h"
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

class ECEncryption
{
public:
  static vector<uint8_t> encrypt(const shared_ptr<ECCurve> &curve, const vector<uint8_t> &publicKey, const vector<uint8_t> &plaintext);
  static vector<uint8_t> decrypt(const shared_ptr<ECCurve> &curve, const vector<uint8_t> &privateKey, const vector<uint8_t> &cyphertext);
};

} // namespace cryptoplus
