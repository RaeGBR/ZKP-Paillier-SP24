#include "gtest/gtest.h"

#include "app/PolynomialCommitment.hpp"
#include "lib/paillier/PaillierEncryption.hpp"
#include "lib/math/Random.hpp"

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

TEST(PolynomialCommitment, PolyCommit_eval_verify)
{
  auto Q = Integer::create("607", 10);
  auto p = Integer::create("101", 10);
  auto g = Integer::create("8", 10);
  size_t m1 = 1;
  size_t m2 = 2;
  size_t n = 2;
  PolynomialCommitment commitScheme(Q, p, g, n);

  vector<shared_ptr<Integer>> poly{
      Integer::create("1", 10),
      Integer::create("3", 10),
      Integer::create("5", 10),
      Integer::create("7", 10),
      Integer::create("9", 10),
      Integer::create("11", 10)};

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

TEST(PolynomialCommitment, PolyCommit_eval_verify2)
{
  int byteLength = 32;
  auto crypto = PaillierEncryption::generate(byteLength);
  auto Q = crypto->Q;
  auto p = crypto->n2;
  auto g = crypto->G;

  // Commitment scheme can support max 10 element as a row
  size_t n = 10;
  PolynomialCommitment commitScheme(Q, p, g, n);

  size_t cnt = 50;    // total polynomial elements count
  size_t m = cnt / n; // m = 5, compress to a matrix which 5 x 10 matrix
  size_t m1 = 3;      // -ve degree -15 to -1 (-n * m1)
  size_t m2 = m - m1; // +ve degree 1 to 35 (n * m2)

  // prepare the polynomial from x^-15, x^-14, ... , x^-1, x, x^2, ... , x^35
  vector<shared_ptr<Integer>> poly;
  for (int i = 0; i < cnt; i++)
  {
    poly.push_back(Random::genInteger(p));
  }

  // prepare m + 1 randomness for commitment
  vector<shared_ptr<Integer>> ri;
  for (int i = 0; i < m + 1; i++)
  {
    ri.push_back(Random::genInteger(p));
  }

  // calculate PolyCommit
  auto T = commitScheme.calcT(m1, m2, n, poly);
  auto pc = commitScheme.commit(m1, m2, n, T, ri);

  // set challenge value
  auto x = Random::genInteger(p);

  // calculate PolyEval
  auto pe = commitScheme.eval(m1, m2, n, T, ri, x);

  // verify
  auto isValid = commitScheme.verify(m1, m2, n, pc, pe, x);
  EXPECT_EQ(isValid, true);

  // calculate value v
  auto v = commitScheme.calcV(n, pe, x);
  EXPECT_TRUE(v->gt(Integer::ZERO()));
}

} // namespace
