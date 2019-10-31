#include "gtest/gtest.h"

#include "Integer.hpp"
#include "lib/math/Random.hpp"

namespace
{

vector<uint8_t> emptySeed;

TEST(RandomUtils, Random_Integer)
{
  auto result1 = Random::genInteger(8, false, emptySeed);
  auto result2 = Random::genInteger(8, false, emptySeed);
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

  auto result1 = Random::genInteger(16, false, seed);
  EXPECT_EQ(result1->toHex(), answer);
  EXPECT_TRUE(result1->lt(Integer::pow2(16 * 8)));

  auto result2 = Random::genInteger(16, false, seed);
  EXPECT_EQ(result2->toHex(), answer);

  const string seedStr2 = "HAHA";
  const auto seed2 = Utils::stringToBinary(seedStr2);
  auto result3 = Random::genInteger(16, false, seed2);
  EXPECT_NE(result3->toHex(), answer);

  auto result4 = Random::genInteger(128, false, seed);
  EXPECT_NE(result4->toHex(), answer);
  EXPECT_TRUE(result4->lt(Integer::pow2(128 * 8)));

  auto result5 = Random::genInteger(16, true, seed);
  EXPECT_NE(result5->toHex(), answer);
  EXPECT_TRUE(result5->lt(Integer::pow2(16 * 8)));
}

TEST(RandomUtils, Random_string_binary_and_integer)
{
  auto seed = Utils::stringToBinary("test");
  int size = 8;
  auto a = Random::genInteger(8, false, seed);
  auto b = Random::genHex(8, seed);
  auto c = Random::genBinary(8, seed);

  EXPECT_EQ(a->toHex(), b);
  EXPECT_EQ(a->toBinary(), c);
}

TEST(RandomUtils, Random_data_length)
{
  for (int i = 1; i < 100; i++)
  {
    auto a = Random::genBinary(i);
    auto b = Random::genHex(i);
    auto c = Random::genInteger(i);
    auto max = Integer::pow2(i * 8);
    EXPECT_EQ(a.size(), i);
    EXPECT_EQ(b.size(), i * 2);
    EXPECT_TRUE(c->lt(max));
  }
}

TEST(Random, getGenerators)
{
  uint32_t n = 3;
  auto p = Integer::create("101", 10);
  auto gs = Random::getGenerators(n, p);

  EXPECT_EQ(gs.size(), n);
  EXPECT_EQ(gs[0]->toString(), "51");
  EXPECT_EQ(gs[1]->toString(), "55");
  EXPECT_EQ(gs[2]->toString(), "32");
}

TEST(Random, getGenerators_all_numbers_in_group)
{
  uint32_t n = 1000;
  auto p = Integer::create("9", 10);
  auto gs = Random::getGenerators(n, p);

  EXPECT_EQ(gs.size(), n);
  for (int i = 0; i < n; i++)
  {
    EXPECT_EQ(gs[i]->lt(p), true);
    EXPECT_EQ(gs[i]->gte(Integer::ZERO()), true);
  }
}

TEST(Random, getRandoms)
{
  uint32_t n = 3;
  auto p = Integer::create("101", 10);
  auto gs1 = Random::getRandoms(n, p);
  auto gs2 = Random::getRandoms(n, p);

  EXPECT_EQ(gs1.size(), n);
  EXPECT_EQ(gs2.size(), n);
  EXPECT_NE(gs1[0]->toString(), gs2[0]->toString());
  EXPECT_NE(gs1[1]->toString(), gs2[1]->toString());
  EXPECT_NE(gs1[2]->toString(), gs2[2]->toString());
}

TEST(Random, getRandoms_all_numbers_in_group)
{
  uint32_t n = 1000;
  auto p = Integer::create("9", 10);
  auto gs = Random::getRandoms(n, p);

  EXPECT_EQ(gs.size(), n);
  for (int i = 0; i < n; i++)
  {
    EXPECT_EQ(gs[i]->lt(p), true);
    EXPECT_EQ(gs[i]->gte(Integer::ZERO()), true);
  }
}

} // namespace
