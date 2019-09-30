#include "gtest/gtest.h"

#include "Integer.hpp"

#include "lib/utils/MathUtils.hpp"

namespace
{

TEST(MathUtils, Complement)
{
  int bits = 8;
  auto x = Integer::createWithString("7b");
  auto y = MathUtils::encodeComplement(x, bits);
  auto z = MathUtils::decodeComplement(y, bits); // Reverse the Complement

  EXPECT_EQ(y->toString(), "251"); // 2^7 + 123 = 251
  EXPECT_EQ(z->toString(), x->toString());
}

TEST(MathUtils, Complement_with_negative)
{
  int bits = 8;
  auto x = Integer::createWithString("-7b");
  auto y = MathUtils::encodeComplement(x, bits);
  auto z = MathUtils::decodeComplement(y, bits); // Reverse the Complement

  EXPECT_EQ(y->toString(), "5"); // 2^7 - 123 = 5
  EXPECT_EQ(z->toString(), x->toString());
}

TEST(MathUtils, Decimal)
{
  string orig, dec;
  shared_ptr<Integer> enc;

  orig = "0";
  enc = MathUtils::encodeDecimal(orig, 1, 0);
  dec = MathUtils::decodeDecimal(enc, 1, 0);
  EXPECT_EQ(orig, dec);
  EXPECT_EQ(enc->toString(), "10");

  orig = "0.50";
  enc = MathUtils::encodeDecimal(orig, 4, 2);
  dec = MathUtils::decodeDecimal(enc, 4, 2);
  EXPECT_EQ(orig, dec);
  EXPECT_EQ(enc->toString(), "10050");

  orig = "-0.50";
  enc = MathUtils::encodeDecimal(orig, 4, 2);
  dec = MathUtils::decodeDecimal(enc, 4, 2);
  EXPECT_EQ(orig, dec);
  EXPECT_EQ(enc->toString(), "9950");
}

} // namespace
