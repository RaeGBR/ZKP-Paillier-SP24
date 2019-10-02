#include "gtest/gtest.h"

#include "Utils.hpp"
#include "CryptoUtils.hpp"
#include "ECCurve.hpp"
#include "KeyPair.hpp"

using namespace std;
using namespace cryptoplus;

namespace
{

const string message = "VEDA PROJECT !";
const auto messageBin = Utils::stringToBinary(message);
const string fakeMsg = "INVALID";
const auto fakeMsgBin = Utils::stringToBinary(fakeMsg);
const auto curve = ECCurve::SECP256K1();
const string seed1 = "key pair 1";
const string seed2 = "key pair 2";
const auto pair1 = KeyPair::createWithSeed(curve, vector<uint8_t>(seed1.begin(), seed1.end()));
const auto pair2 = KeyPair::createWithSeed(curve, vector<uint8_t>(seed2.begin(), seed2.end()));

TEST(ECUtils, ECDSA)
{
  // ECDSA signature is different everytime
  vector<uint8_t> signature1 = CryptoUtils::ecGenerateSignature(curve, pair1->getPrivateKey(), messageBin);
  vector<uint8_t> signature2 = CryptoUtils::ecGenerateSignature(curve, pair2->getPrivateKey(), messageBin);
  vector<uint8_t> signature3 = CryptoUtils::ecGenerateSignature(curve, pair1->getPrivateKey(), messageBin);
  vector<uint8_t> signature4 = CryptoUtils::ecGenerateSignature(curve, pair2->getPrivateKey(), messageBin);

  EXPECT_NE(signature1, signature3);
  EXPECT_NE(signature2, signature4);
  EXPECT_NE(signature1, signature2);
  EXPECT_TRUE(CryptoUtils::ecVerifySignature(curve, pair1->getPublicKey(), messageBin, signature1));
  EXPECT_TRUE(CryptoUtils::ecVerifySignature(curve, pair2->getPublicKey(), messageBin, signature2));
  EXPECT_TRUE(CryptoUtils::ecVerifySignature(curve, pair1->getPublicKey(), messageBin, signature3));
  EXPECT_TRUE(CryptoUtils::ecVerifySignature(curve, pair2->getPublicKey(), messageBin, signature4));
  EXPECT_FALSE(CryptoUtils::ecVerifySignature(curve, pair1->getPublicKey(), messageBin, signature2));
  EXPECT_FALSE(CryptoUtils::ecVerifySignature(curve, pair2->getPublicKey(), messageBin, signature1));
  EXPECT_FALSE(CryptoUtils::ecVerifySignature(curve, pair1->getPublicKey(), fakeMsgBin, signature1));
  EXPECT_FALSE(CryptoUtils::ecVerifySignature(curve, pair2->getPublicKey(), fakeMsgBin, signature2));
}

TEST(ECUtils, ECIES_encrypt_with_public_key)
{
  // ECDSA encryption result is different everytime
  auto cipher1 = CryptoUtils::ecEncrypt(curve, pair1->getPublicKey(), messageBin);
  auto cipher2 = CryptoUtils::ecEncrypt(curve, pair2->getPublicKey(), messageBin);
  auto cipher3 = CryptoUtils::ecEncrypt(curve, pair1->getPublicKey(), messageBin);
  auto cipher4 = CryptoUtils::ecEncrypt(curve, pair2->getPublicKey(), messageBin);

  EXPECT_NE(cipher1, cipher3);
  EXPECT_NE(cipher2, cipher4);
  EXPECT_NE(cipher1, cipher2);
  EXPECT_EQ(CryptoUtils::ecDecrypt(curve, pair1->getPrivateKey(), cipher1), messageBin);
  EXPECT_EQ(CryptoUtils::ecDecrypt(curve, pair2->getPrivateKey(), cipher2), messageBin);
  EXPECT_EQ(CryptoUtils::ecDecrypt(curve, pair1->getPrivateKey(), cipher3), messageBin);
  EXPECT_EQ(CryptoUtils::ecDecrypt(curve, pair2->getPrivateKey(), cipher4), messageBin);
  EXPECT_NE(cipher1, cipher2);
  EXPECT_NE(CryptoUtils::ecDecrypt(curve, pair2->getPrivateKey(), cipher1), messageBin);
  EXPECT_NE(CryptoUtils::ecDecrypt(curve, pair1->getPrivateKey(), cipher2), messageBin);
}

} // namespace
