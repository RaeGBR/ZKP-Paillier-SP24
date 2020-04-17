#include "gtest/gtest.h"

#include "app/namespace.hpp"

#include "app/CircuitZKPVerifier.hpp"
#include "app/CircuitZKPProver.hpp"

namespace
{

TEST(CircuitZKP, Test_1)
{
  // define group element
  auto Q = conv<ZZ>(607);
  auto p = conv<ZZ>(101);
  ZZ_p::init(Q);
  auto g = conv<ZZ_p>(8);

  // define circuit constrains
  auto N = 6;
  auto mnCfg = CircuitZKPVerifier::calcMN(N); // explicitly define a too big circuit
  auto m = mnCfg[0];
  auto n = mnCfg[1];

  vector<shared_ptr<Matrix>> Wqa;
  vector<shared_ptr<Matrix>> Wqb;
  vector<shared_ptr<Matrix>> Wqc;
  Vec<ZZ_p> Kq;
  Kq.SetLength(N);

  cout << "m: " << m << endl;
  cout << "n: " << n << endl;

  ZZ_p::init(p);
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

  // create verifier
  auto verifier = make_shared<CircuitZKPVerifier>(Q, p, g, Wqa, Wqb, Wqc, Kq, m, n, Wqa.size()); // difine the dimension explicitly
  EXPECT_EQ(verifier->Q, Wqa.size());
  EXPECT_EQ(verifier->m, m);
  EXPECT_EQ(verifier->n, n);
  EXPECT_EQ(verifier->N, m * n);
  EXPECT_EQ(verifier->M, m * n + m);

  // prover define / calculate circuit arguments
  shared_ptr<Matrix> A = make_shared<Matrix>(vector<int>({1, 2, 3, 4, 40, 11}))->group(n, m); // make sure the dimension match
  shared_ptr<Matrix> B = make_shared<Matrix>(vector<int>({4, 5, 6, 10, 11, 72}))->group(n, m);
  shared_ptr<Matrix> C = make_shared<Matrix>(vector<int>({4, 10, 18, 40, 36, 85}))->group(n, m);

  // create prover
  auto prover = make_shared<CircuitZKPProver>(verifier, A, B, C);

  // P->V commit
  Vec<ZZ_p> commits;
  prover->commit(commits);

  // V->P challenge y
  verifier->setCommits(commits);
  auto y1 = verifier->calculateY();
  auto y2 = verifier->calculateY();

  EXPECT_EQ(prover->randA.length(), m);
  EXPECT_EQ(prover->randB.length(), m);
  EXPECT_EQ(prover->randC.length(), m);
  EXPECT_EQ(prover->D.length(), n);
  EXPECT_EQ(commits.length(), m * 3 + 1);
  EXPECT_EQ(y1, y2);
  EXPECT_EQ(conv<ZZ>(y1) > 0, true);

  // P get challenge value by his own (non-interactive mode)
  prover->zkp->setCommits(commits);
  auto y3 = prover->zkp->calculateY();

  EXPECT_EQ(y1, y3);

  // P->V polyCommit
  Vec<ZZ_p> pc;
  prover->polyCommit(y3, pc);

  EXPECT_EQ(pc.length(), verifier->txM1 + verifier->txM2 + 1);

  // V->P challenge x
  verifier->setPolyCommits(pc);
  auto x1 = verifier->calculateX();
  auto x2 = verifier->calculateX();

  EXPECT_EQ(x1, x2);
  EXPECT_EQ(conv<ZZ>(x1) > 0, true);

  // P get challenge value by his own (non-interactive mode)
  prover->zkp->setPolyCommits(pc);
  auto x3 = prover->zkp->calculateX();

  EXPECT_EQ(x1, x3);

  // P->V proofs
  Vec<ZZ_p> proofs;
  prover->prove(y3, x3, proofs);

  EXPECT_EQ(proofs.length(), verifier->txN + 1 + verifier->n + 1);

  // V verify
  auto isValid = verifier->verify(proofs, y1, x1);

  EXPECT_TRUE(isValid);

  Vec<ZZ_p> fakeProofs1 = proofs;
  fakeProofs1[0] = 1;
  isValid = verifier->verify(fakeProofs1, y1, x1);

  EXPECT_FALSE(isValid);

  Vec<ZZ_p> fakeProofs2 = proofs;
  fakeProofs2[fakeProofs2.length() - 1] = 1;
  isValid = verifier->verify(fakeProofs2, y1, x1);

  EXPECT_FALSE(isValid);
}

TEST(CircuitZKP, Test_2)
{
  auto Q = conv<ZZ>(607);
  auto p = conv<ZZ>(101);
  ZZ_p::init(Q);
  auto g = conv<ZZ_p>(8);

  // y = x ^ 9
  // 512 = 2 ^ 9
  // 7 = 2 ^ 9 mod 101
  ZZ_p::init(p);
  ZZ_p X = conv<ZZ_p>(2);
  ZZ_p Y = conv<ZZ_p>(512);

  // define circuit constrains
  int m = 2;
  int n = 2;

  vector<shared_ptr<Matrix>> Wqa;
  vector<shared_ptr<Matrix>> Wqb;
  vector<shared_ptr<Matrix>> Wqc;
  Vec<ZZ_p> Kq;
  Kq.SetLength(8);

  auto ZERO_MATRIX = make_shared<Matrix>(m, n);

  Wqa.push_back(make_shared<Matrix>(vector<int>({1, 0, 0, 0}))->group(n));
  Wqa.push_back(make_shared<Matrix>(vector<int>({0, 1, 0, 0}))->group(n));
  Wqa.push_back(ZERO_MATRIX);
  Wqa.push_back(make_shared<Matrix>(vector<int>({0, 0, 1, 0}))->group(n));
  Wqa.push_back(ZERO_MATRIX);
  Wqa.push_back(make_shared<Matrix>(vector<int>({0, 0, 0, 1}))->group(n));
  Wqa.push_back(make_shared<Matrix>(vector<int>({-1, 0, 0, 0}))->group(n));
  Wqa.push_back(ZERO_MATRIX);

  Wqb.push_back(make_shared<Matrix>(vector<int>({-1, 0, 0, 0}))->group(n));
  Wqb.push_back(ZERO_MATRIX);
  Wqb.push_back(make_shared<Matrix>(vector<int>({0, 1, 0, 0}))->group(n));
  Wqb.push_back(ZERO_MATRIX);
  Wqb.push_back(make_shared<Matrix>(vector<int>({0, 0, 1, 0}))->group(n));
  Wqb.push_back(ZERO_MATRIX);
  Wqb.push_back(make_shared<Matrix>(vector<int>({0, 0, 0, 1}))->group(n));
  Wqb.push_back(ZERO_MATRIX);

  Wqc.push_back(ZERO_MATRIX);
  Wqc.push_back(make_shared<Matrix>(vector<int>({-1, 0, 0, 0}))->group(n));
  Wqc.push_back(make_shared<Matrix>(vector<int>({-1, 0, 0, 0}))->group(n));
  Wqc.push_back(make_shared<Matrix>(vector<int>({0, -1, 0, 0}))->group(n));
  Wqc.push_back(make_shared<Matrix>(vector<int>({0, -1, 0, 0}))->group(n));
  Wqc.push_back(make_shared<Matrix>(vector<int>({0, 0, -1, 0}))->group(n));
  Wqc.push_back(ZERO_MATRIX);
  Wqc.push_back(make_shared<Matrix>(vector<int>({0, 0, 0, 1}))->group(n));

  Kq[7] = Y;

  // create verifier
  auto verifier = make_shared<CircuitZKPVerifier>(Q, p, g, Wqa, Wqb, Wqc, Kq);
  EXPECT_EQ(verifier->Q, 8);
  EXPECT_EQ(verifier->m, 2);
  EXPECT_EQ(verifier->n, 2);
  EXPECT_EQ(verifier->N, 4);
  EXPECT_EQ(verifier->M, 6);

  // prover define / calculate circuit arguments
  shared_ptr<Matrix> A = make_shared<Matrix>(vector<int>({2, 4, 16, 256}))->group(n);
  shared_ptr<Matrix> B = make_shared<Matrix>(vector<int>({2, 4, 16, 2}))->group(n);
  shared_ptr<Matrix> C = make_shared<Matrix>(vector<int>({4, 16, 256, 512}))->group(n);

  // create prover
  auto prover = make_shared<CircuitZKPProver>(verifier, A, B, C);

  // P->V commit
  Vec<ZZ_p> commits;
  prover->commit(commits);

  // V->P challenge y
  verifier->setCommits(commits);
  auto y1 = verifier->calculateY();
  auto y2 = verifier->calculateY();

  EXPECT_EQ(prover->randA.length(), m);
  EXPECT_EQ(prover->randB.length(), m);
  EXPECT_EQ(prover->randC.length(), m);
  EXPECT_EQ(prover->D.length(), n);
  EXPECT_EQ(commits.length(), m * 3 + 1);
  EXPECT_EQ(y1, y2);
  EXPECT_EQ(conv<ZZ>(y1) > 0, true);

  // P get challenge value by his own (non-interactive mode)
  prover->zkp->setCommits(commits);
  auto y3 = prover->zkp->calculateY();

  EXPECT_EQ(y1, y3);

  // P->V polyCommit
  Vec<ZZ_p> pc;
  prover->polyCommit(y3, pc);

  EXPECT_EQ(pc.length(), verifier->txM1 + verifier->txM2 + 1);

  // V->P challenge x
  verifier->setPolyCommits(pc);
  auto x1 = verifier->calculateX();
  auto x2 = verifier->calculateX();

  EXPECT_EQ(x1, x2);
  EXPECT_EQ(conv<ZZ>(x1) > 0, true);

  // P get challenge value by his own (non-interactive mode)
  prover->zkp->setPolyCommits(pc);
  auto x3 = prover->zkp->calculateX();

  EXPECT_EQ(x1, x3);

  // P->V proofs
  Vec<ZZ_p> proofs;
  prover->prove(y3, x3, proofs);

  EXPECT_EQ(proofs.length(), verifier->txN + 1 + verifier->n + 1);

  // V verify
  auto isValid = verifier->verify(proofs, y1, x1);

  EXPECT_TRUE(isValid);

  Vec<ZZ_p> fakeProofs1 = proofs;
  fakeProofs1[0] = 1;
  isValid = verifier->verify(fakeProofs1, y1, x1);

  EXPECT_FALSE(isValid);

  Vec<ZZ_p> fakeProofs2 = proofs;
  fakeProofs2[fakeProofs2.length() - 1] = 1;
  isValid = verifier->verify(fakeProofs2, y1, x1);

  EXPECT_FALSE(isValid);
}

TEST(CircuitZKP, Test_3)
{
  // define group element
  auto Q = conv<ZZ>(607);
  auto p = conv<ZZ>(101);
  ZZ_p::init(Q);
  auto g = conv<ZZ_p>(8);

  // c = a * b
  ZZ_p::init(p);
  ZZ_p inputA = conv<ZZ_p>(2);
  ZZ_p inputB = conv<ZZ_p>(3);
  ZZ_p inputC = conv<ZZ_p>(4);
  ZZ_p output = conv<ZZ_p>(24);

  // define circuit constrains
  int m = 1;
  int n = 2;
  vector<shared_ptr<Matrix>> Wqa;
  vector<shared_ptr<Matrix>> Wqb;
  vector<shared_ptr<Matrix>> Wqc;
  Vec<ZZ_p> Kq;
  Kq.SetLength(2);

  auto ZERO_MATRIX = make_shared<Matrix>(m, n);

  Wqa.push_back(make_shared<Matrix>(vector<int>({0, 1}))->group(n));
  Wqb.push_back(make_shared<Matrix>(vector<int>({0, 0}))->group(n));
  Wqc.push_back(make_shared<Matrix>(vector<int>({-1, 0}))->group(n));
  //   Kq.push_back(Integer::ZERO());

  Wqa.push_back(make_shared<Matrix>(vector<int>({0, 0}))->group(n));
  Wqb.push_back(make_shared<Matrix>(vector<int>({0, 0}))->group(n));
  Wqc.push_back(make_shared<Matrix>(vector<int>({0, 1}))->group(n));
  //   Kq.push_back(output);
  Kq[1] = output;

  // create verifier
  auto verifier = make_shared<CircuitZKPVerifier>(Q, p, g, Wqa, Wqb, Wqc, Kq);

  // prover define / calculate circuit arguments
  shared_ptr<Matrix> A = make_shared<Matrix>(vector<int>({2, 6}))->group(n);
  shared_ptr<Matrix> B = make_shared<Matrix>(vector<int>({3, 4}))->group(n);
  shared_ptr<Matrix> C = make_shared<Matrix>(vector<int>({6, 24}))->group(n);

  // create prover
  auto prover = make_shared<CircuitZKPProver>(verifier, A, B, C);

  // P->V commit
  Vec<ZZ_p> commits;
  prover->commit(commits);

  // V->P challenge y
  verifier->setCommits(commits);
  ZZ_p y = conv<ZZ_p>(3);

  // P->V polyCommit
  Vec<ZZ_p> pc;
  prover->polyCommit(y, pc);

  // V->P challenge x
  verifier->setPolyCommits(pc);
  ZZ_p x = conv<ZZ_p>(4);

  // P->V proofs
  Vec<ZZ_p> proofs;
  prover->prove(y, x, proofs);

  EXPECT_EQ(proofs.length(), verifier->txN + 1 + verifier->n + 1);

  // V verify
  auto isValid = verifier->verify(proofs, y, x);

  EXPECT_TRUE(isValid);
}

} // namespace
