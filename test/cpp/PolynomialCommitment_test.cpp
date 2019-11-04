#include "gtest/gtest.h"

#include "app/PolynomialCommitment.hpp"

using namespace std;
using namespace cryptoplus;
using namespace polyu;

namespace
{

TEST(PolynomialCommitment, commit)
{
  auto Q = Integer::create("607", 10);
  auto p = Integer::create("101", 10);
  auto g = Integer::create("8", 10);

  PolynomialCommitment commitScheme(Q, p, g, 2);

  auto r = Integer::create("3", 10);
  vector<shared_ptr<Integer>> mi{
      Integer::create("2", 10),
      Integer::create("1", 10)};

  // 8^3 * 64^2 * 512^1 mod 607
  auto c = commitScheme.commit(mi, r);
  EXPECT_EQ(c->toString(), "100");
}

TEST(PolynomialCommitment, mod)
{
  auto N = Integer::create("7", 10);

  auto a = Integer::create("3", 10);
  auto b = Integer::create("5", 10);
  auto c = a->sub(b)->mod(N);

  EXPECT_EQ(c->toString(), "5");
}

TEST(PolynomialCommitment, commit_eval_verify)
{
  auto Q = Integer::create("607", 10);
  auto p = Integer::create("101", 10);
  auto g = Integer::create("8", 10);
  PolynomialCommitment commitScheme(Q, p, g, 3);

  vector<shared_ptr<Integer>> poly{
      Integer::create("1", 10),
      Integer::create("3", 10),
      Integer::create("5", 10),
      Integer::create("7", 10),
      Integer::create("9", 10),
      Integer::create("11", 10)};

  size_t m1 = 1;
  size_t m2 = 2;
  size_t n = 2;

  vector<shared_ptr<Integer>> ri{
      Integer::create("5", 10),
      Integer::create("6", 10),
      Integer::create("7", 10),
      Integer::create("8", 10)};

  auto T = commitScheme.calcT(m1, m2, n, poly);
  auto pc = commitScheme.commit(m1, m2, n, T, ri);

  auto x = Integer::create("9", 10);

  auto pe = commitScheme.eval(m1, m2, n, T, ri, x);

  auto isValid = commitScheme.verify(m1, m2, n, pc, pe, x);

  EXPECT_EQ(isValid, true);

  auto v = commitScheme.calcV(n, pe, x);
  EXPECT_EQ(v->toString(), "98");
}

} // namespace
