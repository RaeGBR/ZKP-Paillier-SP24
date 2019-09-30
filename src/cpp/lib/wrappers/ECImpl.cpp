#include "ECImpl.hpp"

// #include <stdexcept>

using namespace std;
using namespace cryptoplus;

// Djinni Wrapper
shared_ptr<cryptoplus::EC> cryptoplus::EC::create(
    const shared_ptr<Integer> &p, const shared_ptr<Integer> &a, const shared_ptr<Integer> &b,
    const shared_ptr<Integer> &n, const shared_ptr<ECPoint> &g)
{
  // TODO: How to pass the OID?
  return make_shared<ECImpl>(p, a, b, n, g, CryptoPP::ASN1::secp256k1());
}

shared_ptr<cryptoplus::EC> cryptoplus::EC::SECP256K1()
{
  return cryptoplus::EC::create(
      Integer::createWithString("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2Fh"),
      Integer::createWithString("0"),
      Integer::createWithString("7"),
      Integer::createWithString("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141h"),
      ECPoint::create(
          Integer::createWithString("79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798h"),
          Integer::createWithString("483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8h")));
}

shared_ptr<ECPoint> cryptoplus::EC::getPublicElement(const binary_t &publicKey)
{
  // Turn Keys into CryptoPP Key Instance
  string publicKeyString = Utils::binaryToString(publicKey);
  CryptoPP::ECIES<CryptoPP::ECP>::PublicKey ePublicKey;
  CryptoPP::StringSource pubString(publicKeyString, true);

  ePublicKey.Load(pubString);

  // Get Public Key's Point
  return make_shared<ECPointImpl>(ePublicKey.GetPublicElement());
}

shared_ptr<Integer> cryptoplus::EC::getPrivateElement(const binary_t &privateKey)
{
  string privateKeyString = Utils::binaryToString(privateKey);
  CryptoPP::ECIES<CryptoPP::ECP>::PrivateKey ePrivateKey;
  CryptoPP::StringSource privString(privateKeyString, true);

  ePrivateKey.Load(privString);

  // Get Private Key's Scalar
  CryptoPP::Integer scalar = ePrivateKey.GetPrivateExponent();
  return make_shared<IntegerImpl>(scalar);
}

// Class Implement
ECImpl::ECImpl(
    shared_ptr<Integer> P, shared_ptr<Integer> A, shared_ptr<Integer> B,
    shared_ptr<Integer> N, shared_ptr<ECPoint> G, CryptoPP::OID oid)
{
  _P = P;
  _A = A;
  _B = B;
  _N = N;
  _G = G;

  _curve = make_shared<CryptoPP::ECP>(
      *(dynamic_pointer_cast<IntegerImpl>(P)->getValue()),
      *(dynamic_pointer_cast<IntegerImpl>(A)->getValue()),
      *(dynamic_pointer_cast<IntegerImpl>(B)->getValue()));
  _oid = oid;
}

shared_ptr<Integer> ECImpl::getP()
{
  return _P;
}

shared_ptr<Integer> ECImpl::getA()
{
  return _A;
}

shared_ptr<Integer> ECImpl::getB()
{
  return _B;
}

shared_ptr<Integer> ECImpl::getN()
{
  return _N;
}

shared_ptr<ECPoint> ECImpl::getG()
{
  return _G;
}

shared_ptr<ECPoint> ECImpl::inv(const shared_ptr<ECPoint> &a)
{
  auto _a = dynamic_pointer_cast<ECPointImpl>(a);
  return make_shared<ECPointImpl>(
      _curve->Inverse(
          *(_a->getPoint())));
}

shared_ptr<ECPoint> ECImpl::add(const shared_ptr<ECPoint> &a, const shared_ptr<ECPoint> &b)
{
  auto _a = dynamic_pointer_cast<ECPointImpl>(a);
  auto _b = dynamic_pointer_cast<ECPointImpl>(b);
  return make_shared<ECPointImpl>(
      _curve->Add(
          *(_a->getPoint()),
          *(_b->getPoint())));
}

shared_ptr<ECPoint> ECImpl::sub(const shared_ptr<ECPoint> &a, const shared_ptr<ECPoint> &b)
{
  auto _b = inv(b);
  return add(a, _b);
}

shared_ptr<ECPoint> ECImpl::mul(const shared_ptr<Integer> &a, const shared_ptr<ECPoint> &b)
{
  auto _a = dynamic_pointer_cast<IntegerImpl>(a);
  auto _b = dynamic_pointer_cast<ECPointImpl>(b);
  return make_shared<ECPointImpl>(_curve->Multiply(*(_a->getValue()), *(_b->getPoint())));
}

shared_ptr<ECPoint> ECImpl::multiply(const shared_ptr<Integer> &a, const shared_ptr<ECPoint> &b)
{
  return mul(a, b);
}

bool ECImpl::verify(const shared_ptr<ECPoint> &p)
{
  auto _p = dynamic_pointer_cast<ECPointImpl>(p);
  return _curve->VerifyPoint(*(_p->getPoint()));
}

binary_t ECImpl::encodePoint(const shared_ptr<ECPoint> &p, bool compressed)
{
  auto _p = dynamic_pointer_cast<ECPointImpl>(p);
  size_t size = _curve->EncodedPointSize(compressed);
  size_t bufSize = sizeof(CryptoPP::byte) * size;
  CryptoPP::byte *result;
  result = new CryptoPP::byte[size];
  for (int i = 0; i < size; i++)
    result[i] = 0;
  _curve->EncodePoint(result, *(_p->getPoint()), compressed);

  string encodedPoint;
  CryptoPP::StringSource s(result, bufSize, true, new CryptoPP::StringSink(encodedPoint));

  return Utils::stringToBinary(encodedPoint);
}

shared_ptr<ECPoint> ECImpl::decodePoint(const binary_t &encoded)
{
  CryptoPP::ECPPoint result;
  CryptoPP::byte *b = (CryptoPP::byte *)encoded.data();

  // Use Data String Length (= Byte Length), instead of sizeof(b)
  bool success = _curve->DecodePoint(result, b, encoded.size());

  if (!success)
    invalid_argument("Point Decoding Failure (Mostly invalid Encoding String Length)");
  return make_shared<ECPointImpl>(result);
}

shared_ptr<CryptoPP::ECP> ECImpl::getCurve()
{
  return _curve;
}

CryptoPP::OID ECImpl::getOID()
{
  return _oid;
}
