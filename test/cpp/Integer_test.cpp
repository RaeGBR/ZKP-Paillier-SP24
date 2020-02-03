#include "gtest/gtest.h"

#include "Integer.hpp"
#include "lib/math/IntegerImpl.hpp"

using namespace std;
using namespace cryptoplus;

namespace
{

TEST(IntegerWrapper, Default)
{
  shared_ptr<Integer> a = Integer::ZERO();
  shared_ptr<Integer> b = make_shared<IntegerImpl>("0");
  ASSERT_TRUE(a->eq(b));
  EXPECT_EQ(a->toString(), "0");

  shared_ptr<Integer> c = Integer::ZERO();
  shared_ptr<Integer> d = Integer::ONE();
  ASSERT_TRUE(a->eq(c));
  ASSERT_FALSE(a->eq(d));
  EXPECT_EQ(c->toString(), "0");
  EXPECT_EQ(d->toString(), "1");
}

TEST(IntegerWrapper, Constructor_and_convertor)
{
  string hex = "1234";
  string str = "4660";
  vector<uint8_t> bin{0x12, 0x34};
  auto a = Integer::createWithString(hex);

  EXPECT_EQ(a->toString(), str);
  EXPECT_EQ(a->toHex(), hex);
  EXPECT_EQ(a->toBinary(), bin);

  auto b = Integer::createWithBinary(bin);
  auto c = make_shared<IntegerImpl>(str.c_str(), 10);
  EXPECT_EQ(b->toString(), str);
  EXPECT_EQ(c->toString(), str);
}

TEST(IntegerWrapper, PowerOf2)
{
  shared_ptr<Integer> a = Integer::pow2(0);
  shared_ptr<Integer> b = Integer::pow2(1);
  shared_ptr<Integer> c = Integer::pow2(2);
  shared_ptr<Integer> d = Integer::pow2(8);
  EXPECT_EQ(a->toString(), "1");
  EXPECT_EQ(b->toString(), "2");
  EXPECT_EQ(c->toString(), "4");
  EXPECT_EQ(d->toString(), "256");
}

TEST(IntegerWrapper, DecEqualsHex)
{
  shared_ptr<Integer> a = make_shared<IntegerImpl>("26", 10);
  shared_ptr<Integer> b = make_shared<IntegerImpl>("1A");
  shared_ptr<Integer> c = make_shared<IntegerImpl>("1ah");
  shared_ptr<Integer> d = make_shared<IntegerImpl>("0x1a");
  ASSERT_TRUE(a->eq(b));
  ASSERT_TRUE(a->eq(c));
  ASSERT_TRUE(a->eq(d));
  EXPECT_EQ(a->toString(), "26");
  EXPECT_EQ(a->toHex(), "1A");
}

TEST(IntegerWrapper, Add)
{
  shared_ptr<Integer> a = make_shared<IntegerImpl>("1234", 10);
  shared_ptr<Integer> b = make_shared<IntegerImpl>("766", 10);
  shared_ptr<Integer> c = make_shared<IntegerImpl>("2000", 10);

  ASSERT_TRUE(a->add(b)->eq(c));
}

TEST(IntegerWrapper, Subtract)
{
  shared_ptr<Integer> a = make_shared<IntegerImpl>("2018", 10);
  shared_ptr<Integer> b = make_shared<IntegerImpl>("22", 10);
  shared_ptr<Integer> c = make_shared<IntegerImpl>("1996", 10);

  ASSERT_TRUE(a->sub(b)->eq(c));
}

TEST(IntegerWrapper, Subtract_then_mod)
{
  shared_ptr<Integer> a = make_shared<IntegerImpl>("1", 10);
  shared_ptr<Integer> b = make_shared<IntegerImpl>("5", 10);
  shared_ptr<Integer> c = make_shared<IntegerImpl>("7", 10);
  shared_ptr<Integer> d = make_shared<IntegerImpl>("3", 10);

  ASSERT_TRUE(a->sub(b)->mod(c)->eq(d));
}

TEST(IntegerWrapper, Multiply)
{
  shared_ptr<Integer> a = make_shared<IntegerImpl>("35", 10);
  shared_ptr<Integer> b = make_shared<IntegerImpl>("10", 10);
  shared_ptr<Integer> c = make_shared<IntegerImpl>("350", 10);

  ASSERT_TRUE(a->mul(b)->eq(c));
}

TEST(IntegerWrapper, Division)
{
  shared_ptr<Integer> a = make_shared<IntegerImpl>("40", 10);
  shared_ptr<Integer> b = make_shared<IntegerImpl>("10", 10);
  shared_ptr<Integer> c = make_shared<IntegerImpl>("4", 10);

  shared_ptr<Integer> d = make_shared<IntegerImpl>("35", 10);
  shared_ptr<Integer> e = make_shared<IntegerImpl>("10", 10);
  shared_ptr<Integer> f = make_shared<IntegerImpl>("3", 10);

  ASSERT_TRUE(a->div(b)->eq(c));
  ASSERT_TRUE(d->div(e)->eq(f));
}

TEST(IntegerWrapper, Mod)
{
  shared_ptr<Integer> a = make_shared<IntegerImpl>("130", 10);
  shared_ptr<Integer> b = make_shared<IntegerImpl>("11", 10);
  shared_ptr<Integer> c = make_shared<IntegerImpl>("9", 10);

  ASSERT_TRUE(a->mod(b)->eq(c));
}

TEST(IntegerWrapper, Inverse)
{
  shared_ptr<Integer> I = make_shared<IntegerImpl>("1", 10);
  shared_ptr<Integer> N = make_shared<IntegerImpl>("5", 10);

  shared_ptr<Integer> a = make_shared<IntegerImpl>("2", 10);
  shared_ptr<Integer> b = make_shared<IntegerImpl>("3", 10);

  EXPECT_EQ(a->mul(b)->mod(N)->toString(), I->toString());
  EXPECT_EQ(a->inv(N)->toString(), b->toString());
}

TEST(IntegerWrapper, MultiplyInModular)
{
  shared_ptr<Integer> N = make_shared<IntegerImpl>("11", 10);
  shared_ptr<Integer> a = make_shared<IntegerImpl>("3", 10);
  shared_ptr<Integer> b = make_shared<IntegerImpl>("5", 10);
  shared_ptr<Integer> c;
  shared_ptr<Integer> d;

  // 3*5 = 15 = 4 mod 11
  c = a->modMul(b, N);
  d = b->modMul(a, N);
  EXPECT_EQ(c->toString(), "4");
  EXPECT_EQ(d->toString(), "4");
}

TEST(IntegerWrapper, PowerInModular)
{
  shared_ptr<Integer> N = make_shared<IntegerImpl>("11", 10);
  shared_ptr<Integer> a = make_shared<IntegerImpl>("5", 10);
  shared_ptr<Integer> b = make_shared<IntegerImpl>("2", 10);
  shared_ptr<Integer> c;
  shared_ptr<Integer> d;

  // 5^2 = 25 = 3 mod 11
  // 2^5 = 32 = 10 mod 11
  c = a->modPow(b, N);
  d = b->modPow(a, N);
  EXPECT_EQ(c->toString(), "3");
  EXPECT_EQ(d->toString(), "10");
}

TEST(IntegerWrapper, SquareRootInModular)
{
  shared_ptr<Integer> N = make_shared<IntegerImpl>("11", 10);
  shared_ptr<Integer> a = make_shared<IntegerImpl>("3", 10);
  shared_ptr<Integer> b;
  shared_ptr<Integer> c;

  // 5*5 = 25 = 3 mod 11
  // -5 = 11-5 = 6 mod 11
  // 6*6 = 36 = 3 mod 11
  b = a->modSqrt(N);
  c = N->sub(b);
  EXPECT_EQ(b->toString(), "5");
  EXPECT_EQ(c->toString(), "6");
  EXPECT_EQ(b->mul(b)->mod(N)->toString(), a->toString());
  EXPECT_EQ(c->mul(c)->mod(N)->toString(), a->toString());
}

TEST(IntegerWrapper, ToString)
{
  shared_ptr<Integer> a = make_shared<IntegerImpl>("8216", 10);
  EXPECT_EQ(a->toString(), "8216");
  EXPECT_EQ(a->toHex(), "2018");
}

TEST(IntegerWrapper, FunctionIsPure)
{
  shared_ptr<Integer> a = make_shared<IntegerImpl>("2", 10);
  shared_ptr<Integer> b = make_shared<IntegerImpl>("3", 10);
  shared_ptr<Integer> c;

  // (2 + 3) * 2 mod 3
  c = a->add(b)->mul(a)->mod(b);
  EXPECT_EQ(c->toString(), "1");
  EXPECT_EQ(a->toString(), "2");
  EXPECT_EQ(b->toString(), "3");
}

TEST(IntegerWrapper, LessThanEqual)
{
  shared_ptr<Integer> a = Integer::createWithString("2.");
  shared_ptr<Integer> b = Integer::createWithString("3.");

  EXPECT_TRUE(a->lte(b));
  EXPECT_FALSE(b->lte(a));
}

TEST(IntegerWrapper, GreaterThanEqual)
{
  shared_ptr<Integer> a = Integer::createWithString("2.");
  shared_ptr<Integer> b = Integer::createWithString("3.");

  EXPECT_TRUE(b->gte(a));
  EXPECT_FALSE(a->gte(b));
}

TEST(IntegerWrapper, GCD)
{
  shared_ptr<Integer> a = make_shared<IntegerImpl>("37", 10);
  shared_ptr<Integer> b = make_shared<IntegerImpl>("600", 10);
  shared_ptr<Integer> c = make_shared<IntegerImpl>("20", 10);
  shared_ptr<Integer> d = make_shared<IntegerImpl>("100", 10);
  shared_ptr<Integer> e = make_shared<IntegerImpl>("624129", 10);
  shared_ptr<Integer> f = make_shared<IntegerImpl>("2061517", 10);

  EXPECT_EQ(Integer::ZERO()->gcd(Integer::ZERO())->toString(), "0");
  EXPECT_EQ(Integer::ONE()->gcd(Integer::ONE())->toString(), "1");
  EXPECT_EQ(Integer::ZERO()->gcd(Integer::ONE())->toString(), "1");
  EXPECT_EQ(a->gcd(a)->toString(), a->toString());
  EXPECT_EQ(a->gcd(b)->toString(), "1");
  EXPECT_EQ(c->gcd(d)->toString(), "20");
  EXPECT_EQ(e->gcd(f)->toString(), "18913");
}

TEST(IntegerWrapper, Is_prime)
{
  EXPECT_TRUE(make_shared<IntegerImpl>("2", 10)->isPrime());
  EXPECT_TRUE(make_shared<IntegerImpl>("3", 10)->isPrime());
  EXPECT_TRUE(make_shared<IntegerImpl>("7", 10)->isPrime());
  EXPECT_TRUE(make_shared<IntegerImpl>("101", 10)->isPrime());
  EXPECT_FALSE(make_shared<IntegerImpl>("4", 10)->isPrime());
  EXPECT_FALSE(make_shared<IntegerImpl>("6", 10)->isPrime());
  EXPECT_FALSE(make_shared<IntegerImpl>("9", 10)->isPrime());
  EXPECT_FALSE(make_shared<IntegerImpl>("102", 10)->isPrime());
}

TEST(IntegerWrapper, ToFixedBinary)
{
  string hex = "1234";
  string str = "4660";
  vector<uint8_t> b{0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34};
  auto a = Integer::createWithString(hex);

  EXPECT_EQ(a->toHex(), hex);

  EXPECT_EQ(make_shared<IntegerImpl>(a->toFixedBinary(4))->toHex(), hex);
  EXPECT_EQ(make_shared<IntegerImpl>(a->toFixedBinary(10))->toHex(), hex);
  EXPECT_EQ(make_shared<IntegerImpl>(a->toFixedBinary(13))->toHex(), hex);
  EXPECT_EQ(make_shared<IntegerImpl>(a->toFixedBinary(15))->toHex(), hex);
  EXPECT_EQ(make_shared<IntegerImpl>(a->toFixedBinary(20))->toHex(), hex);

  auto bin = a->toBinary();
  int32_t l = 7;
  for (int i = l - bin.size(); i > 0; i--)
  {
    bin.insert(bin.begin(), 0);
  }
  EXPECT_EQ(bin, b);
  EXPECT_EQ(a->toFixedBinary(7), b);
}

} // namespace
