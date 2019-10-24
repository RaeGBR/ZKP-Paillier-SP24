#include "gtest/gtest.h"

#include "lib/namespace.hpp"

#include "Utils.hpp"
#include "RandomUtils.hpp"

#include "ECCurve.hpp"

#include "lib/algo/PedersenCommitment.hpp"

using namespace cryptoplus;
namespace
{
TEST(PedersenCommitment, Commit_and_verify)
{
  auto curve = ECCurve::SECP256K1();
  auto seed = Utils::hexToBinary("01EF");
  auto G = curve->getG();
  auto H = curve->computeGenerator(seed);
  auto x = RandomUtils::randomInt(32);
  auto r = RandomUtils::randomInt(32);
  auto fakeX = RandomUtils::randomInt(32);
  auto fakeR = RandomUtils::randomInt(32);

  auto c = PedersenCommitment::commit(curve, x, G, r, H);
  auto fakeC = PedersenCommitment::commit(curve, fakeX, G, fakeR, H);

  auto xG = G->mul(curve, x);
  auto rH = H->mul(curve, r);
  auto expected = xG->add(curve, rH);

  EXPECT_TRUE(PedersenCommitment::verify(curve, c, x, G, r, H));
  EXPECT_TRUE(c->eq(expected));
  EXPECT_FALSE(PedersenCommitment::verify(curve, c, fakeX, G, r, H));
  EXPECT_FALSE(PedersenCommitment::verify(curve, c, x, G, fakeR, H));
  EXPECT_FALSE(PedersenCommitment::verify(curve, c, x, G, r, G));
  EXPECT_FALSE(PedersenCommitment::verify(curve, c, x, H, r, H));
  EXPECT_FALSE(PedersenCommitment::verify(curve, fakeC, x, G, r, H));
}

TEST(PedersenCommitment, Speed_test_x10)
{
  auto seed = Utils::stringToBinary("test");
  auto curve = ECCurve::SECP256K1();
  auto G = curve->getG();
  auto H = curve->computeGenerator(seed);

  int times = 10;
  for (int i = 0; i < times; i++)
  {
    auto x = RandomUtils::randomInt(32);
    auto r = RandomUtils::randomInt(32);
    auto c = PedersenCommitment::commit(curve, x, G, r, H);
    EXPECT_TRUE(PedersenCommitment::verify(curve, c, x, G, r, H));
  }
}

TEST(PedersenCommitment, Speed_test_x100)
{
  auto seed = Utils::stringToBinary("test");
  auto curve = ECCurve::SECP256K1();
  auto G = curve->getG();
  auto H = curve->computeGenerator(seed);

  int times = 100;
  for (int i = 0; i < times; i++)
  {
    auto x = RandomUtils::randomInt(32);
    auto r = RandomUtils::randomInt(32);
    auto c = PedersenCommitment::commit(curve, x, G, r, H);
    EXPECT_TRUE(PedersenCommitment::verify(curve, c, x, G, r, H));
  }
}
} // namespace
