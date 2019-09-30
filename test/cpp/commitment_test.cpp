#include "gtest/gtest.h"

#include "lib/namespace.hpp"

#include "Utils.hpp"

#include "EC.hpp"

#include "lib/utils/ECPointGenerator.hpp"
#include "lib/algo/PedersenCommitment.hpp"

using namespace cryptoplus;
namespace
{
TEST(PedersenCommitment, Commit_and_verify)
{
  auto curve = EC::SECP256K1();
  auto G = curve->getG();
  auto H = ECPointGenerator::generate(curve, "01EF", true);
  auto x = Utils::randomInt(32);
  auto r = Utils::randomInt(32);
  auto fakeX = Utils::randomInt(32);
  auto fakeR = Utils::randomInt(32);

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
  auto curve = EC::SECP256K1();
  auto G = curve->getG();
  auto H = ECPointGenerator::generate(curve, "test", true);

  int times = 10;
  for (int i = 0; i < times; i++)
  {
    auto x = Utils::randomInt(32);
    auto r = Utils::randomInt(32);
    auto c = PedersenCommitment::commit(curve, x, G, r, H);
    EXPECT_TRUE(PedersenCommitment::verify(curve, c, x, G, r, H));
  }
}

TEST(PedersenCommitment, Speed_test_x100)
{
  auto curve = EC::SECP256K1();
  auto G = curve->getG();
  auto H = ECPointGenerator::generate(curve, "test", true);

  int times = 100;
  for (int i = 0; i < times; i++)
  {
    auto x = Utils::randomInt(32);
    auto r = Utils::randomInt(32);
    auto c = PedersenCommitment::commit(curve, x, G, r, H);
    EXPECT_TRUE(PedersenCommitment::verify(curve, c, x, G, r, H));
  }
}
} // namespace
