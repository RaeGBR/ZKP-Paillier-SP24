#pragma once

#include "../namespace.hpp"

#include "ECCurve.hpp"
#include "Integer.hpp"
#include "../math/IntegerImpl.hpp"
#include "ECPoint.hpp"
#include "ECPointImpl.hpp"
#include "./ECPointGenerator.hpp"

#include "eccrypto.h"
#include "ecp.h"
#include "oids.h"
#include "hex.h"
#include "Utils.hpp"

namespace cryptoplus
{

/**
 * Elliptic Curve Class
 */
class ECCurveImpl : public ECCurve, public enable_shared_from_this<ECCurveImpl>
{
private:
  shared_ptr<CryptoPP::ECP> _curve;

  shared_ptr<Integer> _P;
  shared_ptr<Integer> _A;
  shared_ptr<Integer> _B;
  shared_ptr<Integer> _N;
  shared_ptr<ECPoint> _G;

  CryptoPP::OID _oid;

public:
  // Constructor
  ECCurveImpl(
      shared_ptr<Integer> P, shared_ptr<Integer> A, shared_ptr<Integer> B,
      shared_ptr<Integer> N, shared_ptr<ECPoint> G, CryptoPP::OID oid);

  // Interface
  std::shared_ptr<Integer> getP();
  std::shared_ptr<Integer> getA();
  std::shared_ptr<Integer> getB();
  std::shared_ptr<Integer> getN();
  std::shared_ptr<ECPoint> getG();
  std::shared_ptr<ECPoint> computeGenerator(const std::vector<uint8_t> &seed);
  std::shared_ptr<ECPoint> inv(const std::shared_ptr<ECPoint> &a);
  std::shared_ptr<ECPoint> add(const std::shared_ptr<ECPoint> &a, const std::shared_ptr<ECPoint> &b);
  std::shared_ptr<ECPoint> sub(const std::shared_ptr<ECPoint> &a, const std::shared_ptr<ECPoint> &b);
  std::shared_ptr<ECPoint> mul(const std::shared_ptr<Integer> &a, const std::shared_ptr<ECPoint> &b);
  std::shared_ptr<ECPoint> multiply(const std::shared_ptr<Integer> &a, const std::shared_ptr<ECPoint> &b);
  bool verify(const std::shared_ptr<ECPoint> &p);
  binary_t encodePoint(const std::shared_ptr<ECPoint> &p, bool compressed);
  std::shared_ptr<ECPoint> decodePoint(const binary_t &encoded);

  // Internal Function
  shared_ptr<CryptoPP::ECP> getCurve();
  CryptoPP::OID getOID();
};

} // namespace cryptoplus
