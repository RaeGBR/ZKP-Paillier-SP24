#include "gtest/gtest.h"

#include "lib/namespace.hpp"

#include "app/CircuitZKPVerifier.hpp"
#include "app/CircuitZKPProver.hpp"
#include "lib/math/IntegerImpl.hpp"
#include "lib/math/Matrix.hpp"

using namespace cryptoplus;
using namespace polyu;

namespace
{

TEST(CircuitZKP, Test_1)
{
  // define group element
  auto Q = make_shared<IntegerImpl>(607);
  auto p = make_shared<IntegerImpl>(101);
  auto g = make_shared<IntegerImpl>(8);

  // define circuit constrains
  int m = 2;
  int n = 3;
  vector<shared_ptr<Matrix>> Wqa;
  vector<shared_ptr<Matrix>> Wqb;
  vector<shared_ptr<Matrix>> Wqc;
  vector<shared_ptr<Integer>> Kq;

  auto ZERO_MATRIX = make_shared<Matrix>(m, n);

  Wqa.push_back(make_shared<Matrix>(vector<int>({0, 0, 0, -1, 0, 0}))->group(n));
  Wqa.push_back(ZERO_MATRIX);
  Wqa.push_back(make_shared<Matrix>(vector<int>({0, 0, 0, 0, -1, 0}))->group(n));
  Wqa.push_back(ZERO_MATRIX);
  Wqa.push_back(make_shared<Matrix>(vector<int>({0, 0, 0, 0, 0, -1}))->group(n));
  Wqa.push_back(ZERO_MATRIX);

  Wqb.push_back(ZERO_MATRIX);
  Wqb.push_back(make_shared<Matrix>(vector<int>({0, 0, 0, -1, 0, 0}))->group(n));
  Wqb.push_back(ZERO_MATRIX);
  Wqb.push_back(make_shared<Matrix>(vector<int>({0, 0, 0, 0, -1, 0}))->group(n));
  Wqb.push_back(ZERO_MATRIX);
  Wqb.push_back(make_shared<Matrix>(vector<int>({0, 0, 0, 0, 0, -1}))->group(n));

  Wqc.push_back(make_shared<Matrix>(vector<int>({1, 0, 0, 0, 0, 0}))->group(n));
  Wqc.push_back(make_shared<Matrix>(vector<int>({0, 1, 0, 0, 0, 0}))->group(n));
  Wqc.push_back(make_shared<Matrix>(vector<int>({0, 0, 0, 1, 0, 0}))->group(n));
  Wqc.push_back(make_shared<Matrix>(vector<int>({0, 0, 4, 1, 0, 0}))->group(n));
  Wqc.push_back(make_shared<Matrix>(vector<int>({0, 0, 4, 1, 0, 0}))->group(n));
  Wqc.push_back(make_shared<Matrix>(vector<int>({0, 0, 4, 0, 0, 0}))->group(n));

  Kq.push_back(Integer::ZERO());
  Kq.push_back(Integer::ZERO());
  Kq.push_back(Integer::ZERO());
  Kq.push_back(Integer::ZERO());
  Kq.push_back(Integer::ZERO());
  Kq.push_back(Integer::ZERO());

  // create verifier
  auto verifier = make_shared<CircuitZKPVerifier>(Q, p, g, Wqa, Wqb, Wqc, Kq);
  EXPECT_EQ(verifier->Q, 6);
  EXPECT_EQ(verifier->m, 2);
  EXPECT_EQ(verifier->n, 3);
  EXPECT_EQ(verifier->N, 6);
  EXPECT_EQ(verifier->M, 8);

  // prover define / calculate circuit arguments
  shared_ptr<Matrix> A = make_shared<Matrix>(vector<int>({1, 2, 3, 4, 40, 11}))->group(n);
  shared_ptr<Matrix> B = make_shared<Matrix>(vector<int>({4, 5, 6, 10, 11, 72}))->group(n);
  shared_ptr<Matrix> C = make_shared<Matrix>(vector<int>({4, 10, 18, 40, 36, 85}))->group(n);

  // create prover
  auto prover = make_shared<CircuitZKPProver>(verifier, A, B, C);

  // P->V commit
  vector<shared_ptr<Integer>> commits = prover->commit();

  // V->P challenge y
  verifier->setCommits(commits);
  auto y1 = verifier->calculateY();
  auto y2 = verifier->calculateY();

  EXPECT_EQ(prover->randA.size(), m);
  EXPECT_EQ(prover->randB.size(), m);
  EXPECT_EQ(prover->randC.size(), m);
  EXPECT_EQ(prover->D.size(), n);
  EXPECT_EQ(commits.size(), m * 3 + 1);
  EXPECT_EQ(y1->toHex(), y2->toHex());
  EXPECT_EQ(y1->gt(Integer::ZERO()), true);

  // P get challenge value by his own (non-interactive mode)
  prover->zkp->setCommits(commits);
  auto y3 = prover->zkp->calculateY();
  prover->zkp->clearCommits(); // clear temp variables in non-interactive mode

  EXPECT_EQ(y1->toHex(), y3->toHex());
}

} // namespace
