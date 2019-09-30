#include "gtest/gtest.h"

#include "Integer.hpp"
#include "lib/utils/RandomUtils.hpp"

namespace
{

vector<uint8_t> emptySeed;

TEST(RandomUtils, Random_Integer)
{
  auto result1 = RandomUtils::genInteger(8, false, emptySeed);
  auto result2 = RandomUtils::genInteger(8, false, emptySeed);
  auto max = Integer::pow2(8 * 8);

  EXPECT_NE(result1->toHex(), result2->toHex());
  EXPECT_TRUE(result1->lt(max));
  EXPECT_TRUE(result2->lt(max));
}

TEST(RandomUtils, Random_Integer_Same_Params_Same_Result)
{
  const string seedStr = "HAHAHA";
  const auto seed = Utils::stringToBinary(seedStr);
  const string answer = "3E0EB07B03CEC22EC11529F5282980A9";

  auto result1 = RandomUtils::genInteger(16, false, seed);
  EXPECT_EQ(result1->toHex(), answer);
  EXPECT_TRUE(result1->lt(Integer::pow2(16 * 8)));

  auto result2 = RandomUtils::genInteger(16, false, seed);
  EXPECT_EQ(result2->toHex(), answer);

  const string seedStr2 = "HAHA";
  const auto seed2 = Utils::stringToBinary(seedStr2);
  auto result3 = RandomUtils::genInteger(16, false, seed2);
  EXPECT_NE(result3->toHex(), answer);

  auto result4 = RandomUtils::genInteger(128, false, seed);
  EXPECT_NE(result4->toHex(), answer);
  EXPECT_TRUE(result4->lt(Integer::pow2(128 * 8)));

  auto result5 = RandomUtils::genInteger(16, true, seed);
  EXPECT_NE(result5->toHex(), answer);
  EXPECT_TRUE(result5->lt(Integer::pow2(16 * 8)));
}

TEST(RandomUtils, Random_string_binary_and_integer)
{
  auto seed = Utils::stringToBinary("test");
  int size = 8;
  auto a = RandomUtils::genInteger(8, false, seed);
  auto b = RandomUtils::genHex(8, seed);
  auto c = RandomUtils::genBinary(8, seed);

  EXPECT_EQ(a->toHex(), b);
  EXPECT_EQ(a->toBinary(), c);
}

TEST(RandomUtils, Random_data_length)
{
  for (int i = 1; i < 100; i++)
  {
    auto a = RandomUtils::genBinary(i);
    auto b = RandomUtils::genHex(i);
    auto c = RandomUtils::genInteger(i);
    auto max = Integer::pow2(i * 8);
    EXPECT_EQ(a.size(), i);
    EXPECT_EQ(b.size(), i * 2);
    EXPECT_TRUE(c->lt(max));
  }
}

} // namespace
