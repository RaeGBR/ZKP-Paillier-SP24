#include "gtest/gtest.h"

#include "app/PolynomialCommitment.hpp"

using namespace std;
using namespace cryptoplus;
using namespace polyu;

namespace
{

TEST(PolynomialCommitment, commit)
{
  auto N = Integer::create("7", 10);
  vector<shared_ptr<Integer>> gi{
      Integer::create("2", 10),
      Integer::create("3", 10),
      Integer::create("5", 10)};

  PolynomialCommitment commitScheme(N, gi);

  auto r = Integer::create("3", 10);
  vector<shared_ptr<Integer>> mi{
      Integer::create("2", 10),
      Integer::create("1", 10)};

  // 2^3 * 3^2 * 5^1 mod 7
  auto c = commitScheme.commit(mi, r);
  EXPECT_EQ(c->toString(), "3");
}

} // namespace
