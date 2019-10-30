#include "gtest/gtest.h"

#include <iostream>
#include <string>
#include "Integer.hpp"
#include "lib/math/Random.hpp"

using namespace std;
using namespace cryptoplus;

int size = 4096 / 8;
auto ZERO = Integer::ZERO();
shared_ptr<Integer> P = Random::genInteger(size / 2, true);
shared_ptr<Integer> Q = Random::genInteger(size / 2, true);
shared_ptr<Integer> N = P->mul(Q);
shared_ptr<Integer> a = Random::genInteger(size);
shared_ptr<Integer> b = Random::genInteger(size);

namespace
{

TEST(IntegerSpeedWrapper, Default)
{
  for (int i = 0; i < 1000; i++)
  {
    a = a->modMul(b, N);
  }

  EXPECT_EQ(a->gt(ZERO), true);
}

} // namespace
