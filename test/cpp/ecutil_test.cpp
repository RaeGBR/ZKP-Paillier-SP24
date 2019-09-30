#include "gtest/gtest.h"

#include "Utils.hpp"
#include "EC.hpp"
#include "KeyPair.hpp"

using namespace std;
using namespace cryptoplus;

namespace
{

const string message = "VEDA PROJECT !";
const auto messageBin = Utils::stringToBinary(message);
const string fakeMsg = "INVALID";
const auto fakeMsgBin = Utils::stringToBinary(fakeMsg);
const auto curve = EC::SECP256K1();
const string seed1 = "key pair 1";
const string seed2 = "key pair 2";
const auto pair1 = KeyPair::createWithSeed(curve, vector<uint8_t>(seed1.begin(), seed1.end()));
const auto pair2 = KeyPair::createWithSeed(curve, vector<uint8_t>(seed2.begin(), seed2.end()));

TEST(ECUtils, ECDSA)
{
  // ECDSA signature is different everytime
  vector<uint8_t> signature1 = Utils::calcSignature(curve, pair1->getPrivateKey(), messageBin);
  vector<uint8_t> signature2 = Utils::calcSignature(curve, pair2->getPrivateKey(), messageBin);
  vector<uint8_t> signature3 = Utils::calcSignature(curve, pair1->getPrivateKey(), messageBin);
  vector<uint8_t> signature4 = Utils::calcSignature(curve, pair2->getPrivateKey(), messageBin);

  EXPECT_NE(signature1, signature3);
  EXPECT_NE(signature2, signature4);
  EXPECT_NE(signature1, signature2);
  EXPECT_TRUE(Utils::verifySignature(curve, pair1->getPublicKey(), messageBin, signature1));
  EXPECT_TRUE(Utils::verifySignature(curve, pair2->getPublicKey(), messageBin, signature2));
  EXPECT_TRUE(Utils::verifySignature(curve, pair1->getPublicKey(), messageBin, signature3));
  EXPECT_TRUE(Utils::verifySignature(curve, pair2->getPublicKey(), messageBin, signature4));
  EXPECT_FALSE(Utils::verifySignature(curve, pair1->getPublicKey(), messageBin, signature2));
  EXPECT_FALSE(Utils::verifySignature(curve, pair2->getPublicKey(), messageBin, signature1));
  EXPECT_FALSE(Utils::verifySignature(curve, pair1->getPublicKey(), fakeMsgBin, signature1));
  EXPECT_FALSE(Utils::verifySignature(curve, pair2->getPublicKey(), fakeMsgBin, signature2));
}

TEST(ECUtils, ECIES_encrypt_with_public_key)
{
  // ECDSA encryption result is different everytime
  auto cipher1 = Utils::ecEncrypt(curve, pair1->getPublicKey(), messageBin);
  auto cipher2 = Utils::ecEncrypt(curve, pair2->getPublicKey(), messageBin);
  auto cipher3 = Utils::ecEncrypt(curve, pair1->getPublicKey(), messageBin);
  auto cipher4 = Utils::ecEncrypt(curve, pair2->getPublicKey(), messageBin);

  EXPECT_NE(cipher1, cipher3);
  EXPECT_NE(cipher2, cipher4);
  EXPECT_NE(cipher1, cipher2);
  EXPECT_EQ(Utils::ecDecrypt(curve, pair1->getPrivateKey(), cipher1), messageBin);
  EXPECT_EQ(Utils::ecDecrypt(curve, pair2->getPrivateKey(), cipher2), messageBin);
  EXPECT_EQ(Utils::ecDecrypt(curve, pair1->getPrivateKey(), cipher3), messageBin);
  EXPECT_EQ(Utils::ecDecrypt(curve, pair2->getPrivateKey(), cipher4), messageBin);
  EXPECT_NE(cipher1, cipher2);
  EXPECT_NE(Utils::ecDecrypt(curve, pair2->getPrivateKey(), cipher1), messageBin);
  EXPECT_NE(Utils::ecDecrypt(curve, pair1->getPrivateKey(), cipher2), messageBin);
}

} // namespace
