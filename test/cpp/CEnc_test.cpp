#include "gtest/gtest.h"

#include "app/namespace.hpp"

#include "app/CBase.hpp"
#include "app/CEnc.hpp"
#include "app/CircuitZKPVerifier.hpp"
#include "app/CircuitZKPProver.hpp"
#include "app/math/Matrix.hpp"
#include "app/utils/Timer.hpp"

namespace
{
int byteLength = 16;
auto crypto = make_shared<PaillierEncryption>(byteLength);
auto GP_Q = crypto -> getGroupQ();
auto GP_P = crypto -> getGroupP();
auto GP_G = crypto -> getGroupG();
auto pk = crypto -> getPublicKey();
auto sk1 = crypto -> getPrivateElement1();
auto sk2 = crypto -> getPrivateElement2();

auto decryptor = make_shared<PaillierEncryption>(pk, sk1, sk2, GP_Q, GP_P, GP_G);
auto encryptor = make_shared<PaillierEncryption>(pk, GP_Q, GP_P, GP_G);

TEST(CEnc, Create_circuit)
{
  ZZ_p::init(GP_P);
  auto pk = conv<ZZ>(101);
  auto c = conv<ZZ_p>(9);
  auto crypto = make_shared<PaillierEncryption>(pk, GP_Q, GP_P, GP_G);

  vector<shared_ptr<Matrix>> Wqa;
  vector<shared_ptr<Matrix>> Wqb;
  vector<shared_ptr<Matrix>> Wqc;
  Vec<ZZ_p> Kq;

  auto circuit = make_shared<CEnc>(crypto);
  circuit->wireUp(c);

  EXPECT_EQ(circuit->Wqa.size(), 23);
  EXPECT_EQ(circuit->Wqb.size(), 23);
  EXPECT_EQ(circuit->Wqc.size(), 23);
  EXPECT_EQ(circuit->Kq.length(), 23);
  EXPECT_EQ(circuit->A->n, 12);
  EXPECT_EQ(circuit->B->n, 12);
  EXPECT_EQ(circuit->C->n, 12);

  EXPECT_EQ(circuit->N, 101);
  EXPECT_EQ(circuit->Wqa[22]->m, 1);
  EXPECT_EQ(circuit->Wqa[22]->n, 12); // N = 11
  EXPECT_EQ(circuit->Kq[0], 101);
  EXPECT_EQ(circuit->Kq[22], 9);
}

TEST(CEnc, Run_circuit)
{
  ZZ_p::init(GP_P);

  cout << "byteLength: " << byteLength << endl;
  cout << "p=N^2: " << NumBytes(GP_P) * 8 << endl;
  cout << "Q: " << NumBytes(GP_Q) * 8 << endl;
  cout << "G: " << NumBytes(conv<ZZ>(GP_G)) * 8 << endl;
  cout << "pk=N: " << NumBytes(pk) * 8 << endl;
  cout << "sk=lamda: " << (NumBytes(sk1) + NumBytes(sk2)) * 8 << endl;

  double totalCreate = 0;
  double totalValueAsign = 0;
  double totalCommit = 0;
  double totalChallenge = 0;
  double totalProve = 0;
  double totalVerify = 0;

  auto msg = conv<ZZ>(123);
  auto rand = conv<ZZ_p>(456);
  auto c = encryptor->encrypt(msg, rand);

  Timer::start("circuit.create");
  auto circuit = make_shared<CEnc>(crypto);
  circuit->wireUp(c);
  totalCreate += Timer::end("circuit.create");

  Timer::start("circuit.run");
  circuit->run(msg, rand);
  totalValueAsign += Timer::end("circuit.run");

  EXPECT_EQ(conv<ZZ>(circuit->Kq[0]), encryptor->getPublicKey());
  EXPECT_EQ(circuit->Kq[circuit->linearCount - 1], c);
  EXPECT_EQ(circuit->C->cell(0, circuit->gateCount - 1), c);

  auto N = circuit->gateCount;
  auto Q = circuit->linearCount;
  auto mnCfg = CircuitZKPVerifier::calcMN(N);
  auto m = mnCfg[0];
  auto n = mnCfg[1];
  cout << "Q," << Q << endl;
  cout << "N," << N << endl;
  cout << "m," << m << endl;
  cout << "n," << n << endl;

  Timer::start("circuit.group");
  circuit->group(n, m);
  totalValueAsign += Timer::end("circuit.group");

  Timer::start("circuit.trim");
  circuit->trim();
  totalValueAsign += Timer::end("circuit.trim");

  auto verifier = make_shared<CircuitZKPVerifier>(
      GP_Q, GP_P, GP_G,
      circuit->Wqa, circuit->Wqb, circuit->Wqc, circuit->Kq,
      m, n, Q);
  auto prover = make_shared<CircuitZKPProver>(verifier, circuit->A, circuit->B, circuit->C);

  Timer::start("commit");
  Vec<ZZ_p> commits;
  prover->commit(commits);
  totalCommit += Timer::end("commit");

  Timer::start("calculateY");
  verifier->setCommits(commits);
  auto y = verifier->calculateY();
  totalChallenge += Timer::end("calculateY");

  Timer::start("polyCommit");
  Vec<ZZ_p> pc;
  prover->polyCommit(y, pc);
  totalProve += Timer::end("polyCommit");

  Timer::start("calculateX");
  verifier->setPolyCommits(pc);
  auto x = verifier->calculateX();
  totalChallenge += Timer::end("calculateX");

  Timer::start("prove");
  Vec<ZZ_p> proofs;
  prover->prove(y, x, proofs);
  totalProve += Timer::end("prove");

  Timer::start("verify");
  auto isValid = verifier->verify(proofs, y, x);
  totalVerify += Timer::end("verify");

  cout << "=====" << endl;
  cout << "circuit create: " << totalCreate << "s" << endl;
  cout << "circuit value assign: " << totalValueAsign << "s" << endl;
  cout << "circuit commit: " << totalCommit << "s" << endl;
  cout << "circuit prove: " << totalChallenge + totalProve << "s" << endl;
  cout << "circuit verify: " << totalChallenge + totalVerify << "s" << endl;

  EXPECT_TRUE(isValid);
}

/*
TEST(CEnc, Speed_test)
{
  const size_t groupTry = 5;
  const size_t runTestMax = 50000;
  int byteLengths[] = {8, 16, 32, 64, 128, 256};
  for (size_t i = 0; i < 5; i++)
  {
    int byteLength = byteLengths[i];
    double mulThroughput = 0;
    double powThroughput = 0;
    size_t qSize = 0;
    size_t pSize = 0;
    size_t mSize = 0;

    for (size_t j = 0; j < groupTry; j++)
    {
      auto crypto = make_shared<PaillierEncryption>(byteLength);
      auto GP_Q = crypto->getGroupQ();
      auto GP_P = crypto->getGroupP();
      auto GP_G = crypto->getGroupG();
      auto pk = crypto->getPublicKey();
      auto sk1 = crypto -> getPrivateElement1();
      auto sk2 = crypto -> getPrivateElement2();
      qSize = max(qSize, GP_Q->toBinary().size() * 8);
      pSize = max(pSize, GP_P->toBinary().size() * 8);
      mSize = max(mSize, pk->toBinary().size() * 8);

      auto a = Random::genInteger(byteLength)->mod(GP_Q);
      auto b = Random::genInteger(byteLength)->mod(GP_Q);
      double diff, throughput;

      size_t runTest = runTestMax;
      Timer::start("mod");
      for (size_t i = 0; i < runTestMax; i++)
      {
        a = a->modMul(b, GP_Q);
      }
      diff = Timer::end("mod", true);
      throughput = runTest / diff;
      mulThroughput += throughput;
      EXPECT_TRUE(a->gt(Integer::ZERO()));

      b = a;
      a = GP_G;
      runTest = runTestMax / byteLength;
      Timer::start("pow");
      for (size_t i = 0; i < runTest; i++)
      {
        a = a->modPow(b, GP_Q);
      }
      diff = Timer::end("pow", true);
      throughput = runTest / diff;
      powThroughput += throughput;
      EXPECT_TRUE(a->gt(Integer::ZERO()));
    }

    cout << "key length: " << mSize << " bit" << endl;
    cout << "group p size: " << pSize << " bit" << endl;
    cout << "group Q size: " << qSize << " bit" << endl;
    cout << "average throughput: " << (mulThroughput / groupTry) << " mul/sec" << endl;
    cout << "average throughput: " << (powThroughput / groupTry) << " pow/sec" << endl;
  }
}
//*/

} // namespace
