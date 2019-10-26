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

}
