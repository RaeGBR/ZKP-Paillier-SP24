#include "gtest/gtest.h"

#include "lib/namespace.hpp"

#include "ECCurve.hpp"

#include "lib/math/IntegerImpl.hpp"
#include "lib/ec/ECCurveImpl.hpp"
#include "lib/ec/ECPointImpl.hpp"
#include "lib/ec/ECKeyPairImpl.hpp"

using namespace std;
using namespace cryptoplus;

namespace
{
TEST(ECWrapper, CurveSecp256k1)
{
  auto curve = ECCurve::SECP256K1();

  EXPECT_EQ(curve->getP()->toHex(), "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F");
  EXPECT_EQ(curve->getA()->toString(), "0");
  EXPECT_EQ(curve->getB()->toString(), "7");
  EXPECT_EQ(curve->getN()->toHex(), "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141");
  EXPECT_EQ(curve->encodePoint(curve->getG(), true), Utils::hexToBinary("0279BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798"));
  EXPECT_EQ(curve->encodePoint(curve->getG(), false), Utils::hexToBinary("0479BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8"));
}

TEST(ECWrapper, ECPointAddMul)
{
  auto curve = ECCurve::SECP256K1();
  auto G = curve->getG();
  shared_ptr<ECPoint> ggg = curve->add(curve->add(G, G), G);
  shared_ptr<ECPoint> g3 = curve->mul(Integer::createWithString("3"), G);

  ASSERT_TRUE(ggg->eq(g3));
  ASSERT_FALSE(ggg->eq(G));

  shared_ptr<ECPoint> gggggg = G->add(curve, G)->add(curve, G)->add(curve, ggg);
  shared_ptr<ECPoint> g6 = curve->mul(Integer::createWithString("2"), ggg);
  ASSERT_TRUE(gggggg->eq(g6));
  ASSERT_FALSE(gggggg->eq(G));
}

TEST(ECWrapper, EC_point_inv_sub)
{
  auto curve = ECCurve::SECP256K1();
  auto G = curve->getG();
  auto gi = curve->inv(G);
  auto g3 = curve->mul(Integer::createWithString("3"), G);
  auto g2 = curve->sub(g3, G);
  auto g1 = curve->add(g2, gi);

  ASSERT_FALSE(gi->eq(G));
  ASSERT_TRUE(g1->eq(G));
  ASSERT_FALSE(g2->eq(G));

  auto g10 = curve->mul(Integer::createWithString("a"), G);
  auto g7 = curve->mul(Integer::createWithString("7"), G);

  ASSERT_TRUE(curve->add(g3, g7)->eq(g10));
  ASSERT_TRUE(curve->sub(g10, g7)->eq(g3));
  ASSERT_TRUE(g10->sub(curve, g3)->eq(g7));

  auto g_i = curve->mul(Integer::ONE()->inv(curve->getN()), G);
  ASSERT_FALSE(gi->eq(g_i));
}

TEST(ECWrapper, ECPointEncodeDecode)
{
  auto curve = ECCurve::SECP256K1();
  auto g1 = curve->decodePoint(Utils::hexToBinary("0279BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798"));
  auto g2 = curve->decodePoint(Utils::hexToBinary("0479BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8"));

  ASSERT_TRUE(g1->eq(curve->getG()));
  ASSERT_TRUE(g2->eq(curve->getG()));
  EXPECT_EQ(curve->encodePoint(g1, true), curve->encodePoint(g2, true));
  EXPECT_EQ(curve->encodePoint(g1, false), curve->encodePoint(g2, false));
  EXPECT_NE(curve->encodePoint(g1, true), curve->encodePoint(g2, false));
  EXPECT_NE(curve->encodePoint(g1, false), curve->encodePoint(g2, true));
  EXPECT_NE(curve->encodePoint(g1->add(curve, g2), true), curve->encodePoint(g2, true));
}

TEST(ECWrapper, ECPointGenerateThenEncodeDecode)
{
  auto curve = ECCurve::SECP256K1();
  string seed = "123";
  auto pair = KeyPair::createWithSeed(curve, vector<uint8_t>(seed.begin(), seed.end()));

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

  EXPECT_EQ(Utils::binaryToHex(encoded), "04D238072CA679C9C71410EDC3276DD099C9A09C83D692E540992D431FFB4158A37BBF04361C785266C4593B8CB29FB7A237D257401F8F26D632C1F81CAA64F494");
  EXPECT_EQ(Utils::binaryToHex(compressed), "02D238072CA679C9C71410EDC3276DD099C9A09C83D692E540992D431FFB4158A3");

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
