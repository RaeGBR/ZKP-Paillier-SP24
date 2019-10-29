#pragma once

#include <fstream>
#include <string>
#include <iostream>

#include "eccrypto.h"
#include "osrng.h"
#include "oids.h"
#include "hex.h"

#include "Integer.hpp"
#include "../math/IntegerImpl.hpp"
#include "ECCurve.hpp"
#include "../ec/ECCurveImpl.hpp"
#include "ECPoint.hpp"
#include "../ec/ECPointImpl.hpp"

#include "../utils/HexUtils.hpp"

using namespace std;
using namespace cryptoplus;

namespace cryptoplus
{

class ECSignature
{
public:
  static vector<uint8_t> sign(const shared_ptr<ECCurve> &curve, const vector<uint8_t> &privateKey, const vector<uint8_t> &message);
  static bool verify(const shared_ptr<ECCurve> &curve, const vector<uint8_t> &publicKey, const vector<uint8_t> &message, const vector<uint8_t> &signature);
};

} // namespace cryptoplus
