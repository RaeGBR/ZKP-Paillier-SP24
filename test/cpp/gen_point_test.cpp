#include "gtest/gtest.h"

#include "Integer.hpp"
#include "ECCurve.hpp"
#include "ECPoint.hpp"
#include "Utils.hpp"

#include "lib/ec/ECPointGenerator.hpp"

using namespace std;
using namespace cryptoplus;

namespace
{

auto curve = ECCurve::SECP256K1();
auto seed1 = Utils::stringToBinary("I AM HELLO");
auto seed2 = Utils::stringToBinary("I AM HELLO2");
auto pt1 = ECPointGenerator::generate(curve, seed1, true);

TEST(ECPointGenerator, Compute)
{

  ASSERT_TRUE(curve->verify(pt1));
  EXPECT_EQ(pt1->getX()->toString(), "37362417530823007500763145795361904230141221075031662505274299995878305304582");
  EXPECT_EQ(pt1->getY()->toString(), "20999534077128741478023273986907969592492234109997820833727717266393423708406");
}

TEST(ECPointGenerator, ControlledBySeed)
{
  auto pt2 = ECPointGenerator::generate(curve, seed1, true);
  auto pt3 = ECPointGenerator::generate(curve, seed2, true);

  ASSERT_TRUE(curve->verify(pt1));
  ASSERT_TRUE(curve->verify(pt2));
  ASSERT_TRUE(curve->verify(pt3));
  ASSERT_TRUE(pt1->eq(pt2));
  ASSERT_FALSE(pt1->eq(pt3));
}

TEST(ECPointGenerator, NegativeRoot)
{
  auto pt2 = ECPointGenerator::generate(curve, seed1, false);

  ASSERT_TRUE(curve->verify(pt1));
  ASSERT_TRUE(curve->verify(pt2));
  ASSERT_FALSE(pt1->eq(pt2));
  ASSERT_EQ(pt1->getX()->toString(), pt2->getX()->toString());
  ASSERT_EQ(pt1->getY()->toString(), curve->getP()->sub(pt2->getY())->toString());
}
} // namespace
