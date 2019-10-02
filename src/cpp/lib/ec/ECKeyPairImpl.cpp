#include "ECKeyPairImpl.hpp"

using namespace std;
using namespace cryptoplus;

// Djinni Wrapper
shared_ptr<KeyPair> KeyPair::create(const shared_ptr<ECCurve> &curve)
{
  vector<uint8_t> emptySeed;
  return make_shared<KeyPairImpl>(curve, emptySeed);
}

shared_ptr<KeyPair> KeyPair::createWithSeed(const shared_ptr<ECCurve> &curve, const vector<uint8_t> &seed)
{
  return make_shared<KeyPairImpl>(curve, seed);
}

shared_ptr<KeyPair> KeyPair::createWithPrivateKey(const shared_ptr<ECCurve> &curve, const vector<uint8_t> &privateKey)
{
  vector<uint8_t> emptyKey;
  return make_shared<KeyPairImpl>(curve, privateKey, emptyKey);
}

// Class Implement

KeyPairImpl::KeyPairImpl(const shared_ptr<ECCurve> &curve, const vector<uint8_t> &seed)
{
  this->curve = curve;
  auto _curve = dynamic_pointer_cast<ECCurveImpl>(curve);

  CryptoPP::ECIES<CryptoPP::ECP>::PrivateKey ePrivateKey;
  CryptoPP::ECIES<CryptoPP::ECP>::PublicKey ePublicKey;

  // Generate private key without seed
  if (seed.size() == 0)
  {
    RandomGenerator prng;
    ePrivateKey.Initialize(prng, _curve->getOID());
  }
  // Generate Private Key with Seed
  else
  {
    // Hash the Seed until the length is fixed
    vector<uint8_t> hash = HashUtils::sha256(seed);
    RandomGenerator rng((CryptoPP::byte *)hash.data(), (size_t)hash.size());
    ePrivateKey.Initialize(rng, _curve->getOID());
  }
  // Generate public key using private key
  ePrivateKey.MakePublicKey(ePublicKey);

  // Write into Public Key's String
  string publicKeyString;
  //CryptoPP::HexEncoder pubEncoder(new CryptoPP::StringSink(publicKeyString));
  CryptoPP::StringSink pubEncoder(publicKeyString);
  ePublicKey.DEREncode(pubEncoder);
  pubEncoder.MessageEnd();
  this->publicKey = vector<uint8_t>(publicKeyString.begin(), publicKeyString.end());
  // Write into Private Key's String
  string privateKeyString;
  //CryptoPP::HexEncoder prvEncoder(new CryptoPP::StringSink(privateKeyString));
  CryptoPP::StringSink prvEncoder(privateKeyString);
  ePrivateKey.DEREncode(prvEncoder);
  prvEncoder.MessageEnd();
  this->privateKey = vector<uint8_t>(privateKeyString.begin(), privateKeyString.end());
}

// Used for generated Keys
KeyPairImpl::KeyPairImpl(const shared_ptr<ECCurve> &curve, const vector<uint8_t> &privateKey, const vector<uint8_t> &publicKey)
{
  this->curve = curve;
  this->privateKey = privateKey;
  this->publicKey = publicKey;
  if (publicKey.size() == 0)
  {
    CryptoPP::ECIES<CryptoPP::ECP>::PrivateKey ePrivateKey;
    CryptoPP::ECIES<CryptoPP::ECP>::PublicKey ePublicKey;
    //string privateKeyString = Utils::binaryToString(privateKey);
    CryptoPP::StringSource privString(privateKey.data(), privateKey.size(), true);
    ePrivateKey.Load(privString);
    ePrivateKey.MakePublicKey(ePublicKey);

    string publicKeyString = Utils::binaryToString(publicKey);
    //CryptoPP::HexEncoder pubEncoder(new CryptoPP::StringSink(publicKeyString));
    CryptoPP::StringSink pubEncoder(publicKeyString);
    ePublicKey.DEREncode(pubEncoder);
    pubEncoder.MessageEnd();
    this->publicKey = vector<uint8_t>(publicKeyString.begin(), publicKeyString.end());
  }
}

shared_ptr<ECCurve> KeyPairImpl::getCurve()
{
  return curve;
}

vector<uint8_t> KeyPairImpl::getPrivateKey()
{
  return privateKey;
}

vector<uint8_t> KeyPairImpl::getPublicKey()
{
  return publicKey;
}

shared_ptr<ECPoint> KeyPairImpl::getPublicElement()
{
  return ECCurve::getPublicElement(publicKey);
}

shared_ptr<Integer> KeyPairImpl::getPrivateElement()
{
  return ECCurve::getPrivateElement(privateKey);
}

bool KeyPairImpl::eq(const std::shared_ptr<KeyPair> &b)
{
  return publicKey == b->getPublicKey() &&
         privateKey == b->getPrivateKey();
}
