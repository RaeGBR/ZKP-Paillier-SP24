#include "gtest/gtest.h"

#include "lib/namespace.hpp"

#include "lib/ec/ECCurveImpl.hpp"
#include "lib/ec/ECKeyPairImpl.hpp"
#include "lib/ec/ECPointImpl.hpp"
#include "lib/math/IntegerImpl.hpp"
#include "lib/utils/HexUtils.hpp"

namespace
{
TEST(ECWrapper, ECPointGenerateThenEncodeDecode)
{
  auto curve = ECCurve::SECP256K1();
  string seed = "123";
  auto pair = KeyPair::createWithSeed(curve, HexUtils::stringToBinary(seed));

  // Check The Point is constant first
  auto point = pair->getPublicElement();

  auto validPoint = ECPoint::create(
      make_shared<IntegerImpl>("95084691154107553143451022208308248303354503594730205100282340505731526121635", 10),
      make_shared<IntegerImpl>("55971977230935186082696676635279781284134272441480468508233708197423572382868", 10));
  EXPECT_EQ(point->getX()->toString(), validPoint->getX()->toString());
  EXPECT_EQ(point->getY()->toString(), validPoint->getY()->toString());

  // Check the Encoding is Constant first
  binary_t compressed = curve->encodePoint(point, true);
  binary_t encoded = curve->encodePoint(point, false);

  EXPECT_EQ(HexUtils::binaryToHex(encoded), "04D238072CA679C9C71410EDC3276DD099C9A09C83D692E540992D431FFB4158A37BBF04361C785266C4593B8CB29FB7A237D257401F8F26D632C1F81CAA64F494");
  EXPECT_EQ(HexUtils::binaryToHex(compressed), "02D238072CA679C9C71410EDC3276DD099C9A09C83D692E540992D431FFB4158A3");

  // Decode Compressed Point
  auto result = curve->decodePoint(compressed);
  EXPECT_EQ(point->getX()->toString(), result->getX()->toString());
  EXPECT_EQ(point->getY()->toString(), result->getY()->toString());

  // Decode Encoded Point
  auto result2 = curve->decodePoint(encoded);
  EXPECT_EQ(point->getX()->toString(), result2->getX()->toString());
  EXPECT_EQ(point->getY()->toString(), result2->getY()->toString());
}
} // namespace
