#pragma once

#include "../namespace.hpp"

#include <string>
#include <iostream>

#include "eccrypto.h"
#include "osrng.h"
#include "oids.h"
#include "hex.h"

#include "KeyPair.hpp"
#include "EC.hpp"
#include "ECImpl.hpp"
#include "ECPoint.hpp"
#include "Integer.hpp"
#include "Utils.hpp"

#include "../utils/RandomGenerator.hpp"

using namespace std;

namespace cryptoplus
{

/**
 * KeyPairImpl Class
 * A EC KeyPairImpl must referenced on specified Curcve only.
 */
class KeyPairImpl : public KeyPair
{
private:
  shared_ptr<EC> curve;
  vector<uint8_t> privateKey;
  vector<uint8_t> publicKey;

public:
  // Constructor
  KeyPairImpl(const shared_ptr<EC> &curve, const vector<uint8_t> &seed);
  KeyPairImpl(const shared_ptr<EC> &curve, const vector<uint8_t> &privateKey, const vector<uint8_t> &publicKey);

  // Interface
  std::shared_ptr<EC> getCurve();
  vector<uint8_t> getPrivateKey();
  vector<uint8_t> getPublicKey();
  std::shared_ptr<ECPoint> getPublicElement();
  std::shared_ptr<Integer> getPrivateElement();
  bool eq(const std::shared_ptr<KeyPair> &b);

  // Internal Function
};

} // namespace cryptoplus
