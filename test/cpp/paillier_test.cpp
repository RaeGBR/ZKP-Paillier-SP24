#include "gtest/gtest.h"

#include "lib/namespace.hpp"

#include "lib/paillier/PaillierEncryption.hpp"

using namespace std;
using namespace cryptoplus;

namespace
{
TEST(Paillier, KeyGeneration)
{
  int byteLength = 32;
  for (int i = 0 ; i < 50 ; i++) {
    auto crypto = PaillierEncryption::generate(byteLength);
    EXPECT_EQ(crypto->getPublicKey()->toBinary().size(), byteLength);
    EXPECT_TRUE(crypto->getPublicKey()->gcd(crypto->getPrivateKey())->eq(Integer::ONE()));
  }

  vector<uint8_t> seed{0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34};
  auto c1 = PaillierEncryption::generate(byteLength, seed);
  auto c2 = PaillierEncryption::generate(byteLength, seed);
  EXPECT_TRUE(c2->getPrivateKey()->eq(c1->getPrivateKey()));
  EXPECT_TRUE(c2->getPublicKey()->eq(c1->getPublicKey()));
  EXPECT_TRUE(c1->getPublicKey()->gcd(c1->getPrivateKey())->eq(Integer::ONE()));
}

TEST(Paillier, EncryptionDecryption)
{
  int byteLength = 32;
  auto m = Integer::createWithString("30.");

  auto crypto = PaillierEncryption::generate(byteLength);

  for (int i = 0 ; i < 50 ; i++) {
    auto c = crypto->encrypt(m);
    EXPECT_EQ(crypto->decrypt(c)->toString(), m->toString());
  }
}

TEST(Paillier, HomomorphicAddition)
{
  int byteLength = 32;
  auto m1 = make_shared<IntegerImpl>("30", 10);
  auto m2 = make_shared<IntegerImpl>("32", 10);

  auto crypto = PaillierEncryption::generate(byteLength);

  EXPECT_EQ(crypto->decrypt(crypto->add(crypto->encrypt(m1), m2, false))->toString(), "62");
  EXPECT_EQ(crypto->decrypt(crypto->add(crypto->encrypt(m1), crypto->encrypt(m2), true))->toString(), "62");

  for (int i = 0 ; i < 50 ; i++) {
    auto ma = Random::genInteger(32);
    auto mb = Random::genInteger(32);
    auto sum = ma->add(mb)->mod(crypto->getPublicKey());
    EXPECT_EQ(crypto->decrypt(crypto->add(crypto->encrypt(ma), mb, false))->toString(), sum->toString());
    EXPECT_EQ(crypto->decrypt(crypto->add(crypto->encrypt(ma), crypto->encrypt(mb), true))->toString(), sum->toString());
  }
}

TEST(Paillier, HomomorphicMultiplication)
{
  int byteLength = 32;
  auto m1 = make_shared<IntegerImpl>("30", 10);
  auto m2 = make_shared<IntegerImpl>("32", 10);

  auto crypto = PaillierEncryption::generate(byteLength);

  EXPECT_EQ(crypto->decrypt(crypto->mul(crypto->encrypt(m1), m2))->toString(), "960");

  for (int i = 0 ; i < 50 ; i++) {
    auto ma = Random::genInteger(32);
    auto mb = Random::genInteger(32);
    auto prod = ma->mul(mb)->mod(crypto->getPublicKey());

    EXPECT_EQ(crypto->decrypt(crypto->mul(crypto->encrypt(ma), mb))->toString(), prod->toString());
  }
}
}
