#include "gtest/gtest.h"

#include "lib/namespace.hpp"

#include "app/CBase.hpp"
#include "app/CBatchEnc.hpp"
#include "app/CircuitZKPVerifier.hpp"
#include "app/CircuitZKPProver.hpp"
#include "lib/math/IntegerImpl.hpp"
#include "lib/math/Matrix.hpp"
#include "lib/paillier/PaillierEncryption.hpp"
#include "lib/utils/Timer.hpp"

using namespace cryptoplus;
using namespace polyu;

namespace
{

TEST(CBatchEnc, Create_circuit)
{
  int byteLength = 256;
  auto crypto = PaillierEncryption::generate(byteLength);
  auto GP_Q = crypto->getGroupQ();
  auto GP_P = crypto->getGroupP();
  auto GP_G = crypto->getGroupG();
  auto pk = crypto->getPublicKey();
  auto sk = crypto->getPrivateKey();

  size_t msgCount = 150;
  size_t rangeProofCount = 3;
  size_t slotSize = 4;
  size_t msgPerBatch = 15;

  auto circuit = make_shared<CBatchEnc>(crypto, msgCount, rangeProofCount, slotSize, msgPerBatch);

  EXPECT_EQ(circuit->msgSize, 256);
  EXPECT_EQ(circuit->msgCount, 150);
  EXPECT_EQ(circuit->msgPerBatch, 15);
  EXPECT_EQ(circuit->batchCount, 10);
  EXPECT_EQ(circuit->slotSize, 4);
  EXPECT_EQ(circuit->slotsPerMsg, 64);
  EXPECT_EQ(circuit->rangeProofCount, 3);
  EXPECT_EQ(circuit->RjMax->toHex(), Integer::TWO()->pow(make_shared<IntegerImpl>(282))->toHex());
}

TEST(CBatchEnc, Batch_encrypt_data)
{
  // define crypto params
  int byteLength = 8;
  auto crypto = PaillierEncryption::generate(byteLength);
  auto GP_Q = crypto->getGroupQ();
  auto GP_P = crypto->getGroupP();
  auto GP_G = crypto->getGroupG();
  auto pk = crypto->getPublicKey();
  auto sk = crypto->getPrivateKey();

  auto decryptor = make_shared<PaillierEncryption>(pk, sk, GP_Q, GP_P, GP_G);
  auto encryptor = make_shared<PaillierEncryption>(pk, GP_Q, GP_P, GP_G);

  // define encryption circuit params
  size_t msgCount = 4;
  size_t rangeProofCount = 3;
  size_t slotSize = 2; // 2 byte a slog, "0001"
  size_t msgPerBatch = 3;

  auto proverCir = make_shared<CBatchEnc>(decryptor, msgCount, rangeProofCount, slotSize, msgPerBatch);
  auto verifierCir = make_shared<CBatchEnc>(encryptor, msgCount, rangeProofCount, slotSize, msgPerBatch);

  // P->V: prover batch encrypt message
  vector<shared_ptr<Integer>> msg;
  msg.push_back(make_shared<IntegerImpl>("0001000100010001", 16));
  msg.push_back(make_shared<IntegerImpl>("0000000100010001", 16));
  msg.push_back(make_shared<IntegerImpl>("0000000000010001", 16));
  msg.push_back(make_shared<IntegerImpl>("0001000000000000", 16));

  proverCir->encrypt(msg);

  EXPECT_EQ(proverCir->m.size(), msgCount);
  EXPECT_EQ(proverCir->Rm.size(), msgCount);
  EXPECT_EQ(proverCir->Cm.size(), msgCount);
  EXPECT_EQ(proverCir->Rj.size(), rangeProofCount);
  EXPECT_EQ(proverCir->RRj.size(), rangeProofCount);
  EXPECT_EQ(proverCir->CRj.size(), rangeProofCount);
  EXPECT_EQ(proverCir->m_.size(), 2);
  EXPECT_EQ(proverCir->Rm_.size(), 2);
  EXPECT_EQ(proverCir->Cm_.size(), 2);

  EXPECT_EQ(proverCir->m_[0]->toString(), make_shared<IntegerImpl>("01101111111", 2)->toString());
  EXPECT_EQ(proverCir->m_[1]->toString(), make_shared<IntegerImpl>("1000", 2)->toString());
  EXPECT_EQ(decryptor->decrypt(proverCir->Cm[0])->toBinaryString(), msg[0]->toBinaryString());
  EXPECT_EQ(decryptor->decrypt(proverCir->Cm[1])->toBinaryString(), msg[1]->toBinaryString());
  EXPECT_EQ(decryptor->decrypt(proverCir->Cm[2])->toBinaryString(), msg[2]->toBinaryString());
  EXPECT_EQ(decryptor->decrypt(proverCir->Cm[3])->toBinaryString(), msg[3]->toBinaryString());
  EXPECT_EQ(decryptor->decrypt(proverCir->Cm_[0])->toBinaryString(), proverCir->m_[0]->toBinaryString());
  EXPECT_EQ(decryptor->decrypt(proverCir->Cm_[1])->toBinaryString(), proverCir->m_[1]->toBinaryString());
  EXPECT_EQ(decryptor->decrypt(proverCir->CRj[0])->toBinaryString(), proverCir->Rj[0]->toBinaryString());
  EXPECT_EQ(decryptor->decrypt(proverCir->CRj[1])->toBinaryString(), proverCir->Rj[1]->toBinaryString());
  EXPECT_EQ(decryptor->decrypt(proverCir->CRj[2])->toBinaryString(), proverCir->Rj[2]->toBinaryString());

  // V->P: verifier receive cipher and calculate challenge (lj)
  auto Cm = proverCir->Cm;
  auto Cm_ = proverCir->Cm_;
  auto CRj = proverCir->CRj;

  verifierCir->setCipher(Cm, Cm_, CRj);
  auto ljir1 = verifierCir->calculateLjir();
  auto ljir2 = verifierCir->calculateLjir();

  EXPECT_EQ(ljir1.size(), 6);
  EXPECT_EQ(ljir1, ljir2);

  // P: non-interactive mode, prover calculate challenge value by itself
  auto ljir3 = proverCir->calculateLjir();

  EXPECT_EQ(ljir1, ljir3);

  // P->V: prover compute Lj for range proof
  auto Lj = proverCir->calculateLj(ljir1);

  EXPECT_EQ(Lj.size(), rangeProofCount);

  // P+V: create circuit constrains
  verifierCir->wireUp(ljir1, Lj);
  proverCir->wireUp(ljir1, Lj);

  // P: assign circuit values
  proverCir->run(ljir1, Lj);

  EXPECT_EQ(verifierCir->gateCount, proverCir->gateCount);
  EXPECT_EQ(verifierCir->linearCount, proverCir->linearCount);

  // P+V: setup ZKP protocol for the circuit
  auto N = proverCir->gateCount;
  auto Q = proverCir->linearCount;
  auto mnCfg = CircuitZKPVerifier::calcMN(N);
  auto m = mnCfg[0];
  auto n = mnCfg[1];

  verifierCir->group(n, m);
  verifierCir->trim();
  proverCir->group(n, m);
  proverCir->trim();

  auto verifier = make_shared<CircuitZKPVerifier>(
      GP_Q, GP_P, GP_G,
      verifierCir->Wqa, verifierCir->Wqb, verifierCir->Wqc, verifierCir->Kq,
      m, n);
  auto proverZkp = make_shared<CircuitZKPVerifier>(
      GP_Q, GP_P, GP_G,
      proverCir->Wqa, proverCir->Wqb, proverCir->Wqc, proverCir->Kq,
      m, n);
  auto prover = make_shared<CircuitZKPProver>(proverZkp, proverCir->A, proverCir->B, proverCir->C);

  // P->V: prover commit the circuit arguments
  vector<shared_ptr<Integer>> commits;
  prover->commit(commits);

  // V->P: verifier calculate challenge value Y
  verifier->setCommits(commits);
  auto y = verifier->calculateY();

  // P->V: prover perform polyComit
  vector<shared_ptr<Integer>> pc;
  prover->polyCommit(y, pc);

  // V->P: verifier calculate challenge value X
  verifier->setPolyCommits(pc);
  auto x = verifier->calculateX();

  // P->V: prover calculate the ZKP
  vector<shared_ptr<Integer>> proofs;
  prover->prove(y, x, proofs);

  // V: verify the proof
  auto isValid = verifier->verify(proofs, y, x);

  EXPECT_TRUE(isValid);
}

} // namespace
