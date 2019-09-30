#include "ECSignature.hpp"

vector<uint8_t> ECSignature::sign(const shared_ptr<EC> &curve, const vector<uint8_t> &privateKey, const vector<uint8_t> &message)
{
  auto _curve = dynamic_pointer_cast<ECImpl>(curve);
  auto skEl = EC::getPrivateElement(privateKey);
  auto _skEl = dynamic_pointer_cast<IntegerImpl>(skEl);

  // Initialize Private Key Instance
  CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::PrivateKey sk;

  // Initialize the Private Key with Decoded EC Point
  sk.Initialize(_curve->getOID(), *(_skEl->getValue()));

  // Create Signer Instance
  CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::Signer signer(sk);

  CryptoPP::AutoSeededRandomPool prng;

  size_t siglen = signer.MaxSignatureLength();
  string signature(siglen, 0x00);
  siglen = signer.SignMessage(prng, (CryptoPP::byte *)message.data(), message.size(), (CryptoPP::byte *)signature.data());
  signature.resize(siglen);

  return HexUtils::stringToBinary(signature);
  // string signature = "";

  // // Filter & Sign the Message
  // CryptoPP::StringSource s(
  //     message, true /*pump all*/,
  //     new CryptoPP::SignerFilter(prng, signer, HexUtils::getStringSink(signature)));
  // return HexUtils::stringToBinary(signature);
}

bool ECSignature::verify(const shared_ptr<EC> &curve, const vector<uint8_t> &publicKey, const vector<uint8_t> &message, const vector<uint8_t> &signature)
{
  auto _curve = dynamic_pointer_cast<ECImpl>(curve);
  auto pkEl = EC::getPublicElement(publicKey);
  auto _pkEl = dynamic_pointer_cast<ECPointImpl>(pkEl);

  // Initialize Public Key Instance
  CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::PublicKey pk;

  // Initialize the Public Key with Decoded EC Point
  pk.Initialize(_curve->getOID(), *(_pkEl->getPoint()));

  // Create Verifier Instance
  CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::Verifier verifier(pk);

  bool result = verifier.VerifyMessage((CryptoPP::byte *)message.data(), message.size(), (CryptoPP::byte *)signature.data(), signature.size());

  // // Filter & Verify the Signature
  // CryptoPP::StringSource ss(
  //     HexUtils::binaryToString(signature) + message, true,
  //     new CryptoPP::SignatureVerificationFilter(
  //         verifier, new CryptoPP::ArraySink(
  //                       (CryptoPP::byte *)&result, sizeof(result))));

  return result;
}
