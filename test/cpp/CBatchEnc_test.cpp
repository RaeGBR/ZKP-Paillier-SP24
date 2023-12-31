#include "gtest/gtest.h"

#include "app/namespace.hpp"

#include "app/CBase.hpp"
#include "app/CBatchEnc.hpp"
#include "app/CircuitZKPVerifier.hpp"
#include "app/CircuitZKPProver.hpp"
#include "app/PaillierEncryption.hpp"
#include "app/utils/ConvertUtils.hpp"
#include "app/math/Matrix.hpp"
#include "app/utils/Timer.hpp"

namespace
{

TEST(CBatchEnc, Create_circuit)
{
  int byteLength = 256;
  auto crypto = make_shared<PaillierEncryption>(byteLength);
  auto GP_Q = crypto->getGroupQ();
  auto GP_P = crypto->getGroupP();
  ZZ_p::init(GP_Q);
  auto GP_G = crypto->getGroupG();
  auto pk = crypto->getPublicKey();
  auto sk1 = crypto->getPrivateElement1();
  auto sk2 = crypto->getPrivateElement2();
  ZZ_p::init(GP_P);

  size_t msgCount = 150;
  size_t rangeProofCount = 3;
  size_t slotSize = 4;
  size_t msgPerBatch = 15;

  auto circuit = make_shared<CBatchEnc>(crypto, msgCount, rangeProofCount, slotSize, msgPerBatch);

  ZZ rjMax = (conv<ZZ>(2) << (281)); // 2^282

  EXPECT_EQ(circuit->msgSize, 256);
  EXPECT_EQ(circuit->msgCount, 150);
  EXPECT_EQ(circuit->msgPerBatch, 15);
  EXPECT_EQ(circuit->batchCount, 10);
  EXPECT_EQ(circuit->slotSize, 4);
  EXPECT_EQ(circuit->slotsPerMsg, 64);
  EXPECT_EQ(circuit->rangeProofCount, 3);
  EXPECT_EQ(circuit->RjMax, rjMax);
}

TEST(CBatchEnc, Batch_encrypt_data)
{
  // define crypto params
  int byteLength = 8;
  auto crypto = make_shared<PaillierEncryption>(byteLength);
  auto GP_Q = crypto->getGroupQ();
  auto GP_P = crypto->getGroupP();
  ZZ_p::init(GP_Q);
  auto GP_G = crypto->getGroupG();
  auto pk = crypto->getPublicKey();
  auto sk1 = crypto->getPrivateElement1();
  auto sk2 = crypto->getPrivateElement2();
  ZZ_p::init(GP_P);

  auto decryptor = make_shared<PaillierEncryption>(pk, sk1, sk2, GP_Q, GP_P, GP_G);
  auto encryptor = make_shared<PaillierEncryption>(pk, GP_Q, GP_P, GP_G);

  // define encryption circuit params
  size_t msgCount = 4;
  size_t rangeProofCount = 3;
  size_t slotSize = 2; // 2 byte a slog, "0001"
  size_t msgPerBatch = 3;

  auto proverCir = make_shared<CBatchEnc>(decryptor, msgCount, rangeProofCount, slotSize, msgPerBatch);
  auto verifierCir = make_shared<CBatchEnc>(encryptor, msgCount, rangeProofCount, slotSize, msgPerBatch);

  // P->V: prover batch encrypt message
  Vec<ZZ> msg;
  msg.append(ConvertUtils::hexToZZ("0001000100010001"));
  msg.append(ConvertUtils::hexToZZ("0000000100010001"));
  msg.append(ConvertUtils::hexToZZ("0000000000010001"));
  msg.append(ConvertUtils::hexToZZ("0001000000000000"));

  proverCir->encrypt(msg);

  EXPECT_EQ(proverCir->m.length(), msgCount);
  EXPECT_EQ(proverCir->Rm.length(), msgCount);
  EXPECT_EQ(proverCir->Cm.length(), msgCount);
  EXPECT_EQ(proverCir->Rj.length(), rangeProofCount);
  EXPECT_EQ(proverCir->RRj.length(), rangeProofCount);
  EXPECT_EQ(proverCir->CRj.length(), rangeProofCount);
  EXPECT_EQ(proverCir->m_.length(), 2);
  EXPECT_EQ(proverCir->Rm_.length(), 2);
  EXPECT_EQ(proverCir->Cm_.length(), 2);

  EXPECT_EQ(proverCir->m_[0], ConvertUtils::binaryStringToZZ("001101111111"));
  EXPECT_EQ(proverCir->m_[1], ConvertUtils::binaryStringToZZ("1000"));
  EXPECT_EQ(ConvertUtils::toBinaryString(decryptor->decrypt(proverCir->Cm[0])), ConvertUtils::toBinaryString(msg[0]));
  EXPECT_EQ(ConvertUtils::toBinaryString(decryptor->decrypt(proverCir->Cm[1])), ConvertUtils::toBinaryString(msg[1]));
  EXPECT_EQ(ConvertUtils::toBinaryString(decryptor->decrypt(proverCir->Cm[2])), ConvertUtils::toBinaryString(msg[2]));
  EXPECT_EQ(ConvertUtils::toBinaryString(decryptor->decrypt(proverCir->Cm[3])), ConvertUtils::toBinaryString(msg[3]));
  EXPECT_EQ(ConvertUtils::toBinaryString(decryptor->decrypt(proverCir->Cm_[0])), ConvertUtils::toBinaryString(proverCir->m_[0]));
  EXPECT_EQ(ConvertUtils::toBinaryString(decryptor->decrypt(proverCir->Cm_[1])), ConvertUtils::toBinaryString(proverCir->m_[1]));
  EXPECT_EQ(ConvertUtils::toBinaryString(decryptor->decrypt(proverCir->CRj[0])), ConvertUtils::toBinaryString(proverCir->Rj[0]));
  EXPECT_EQ(ConvertUtils::toBinaryString(decryptor->decrypt(proverCir->CRj[1])), ConvertUtils::toBinaryString(proverCir->Rj[1]));
  EXPECT_EQ(ConvertUtils::toBinaryString(decryptor->decrypt(proverCir->CRj[2])), ConvertUtils::toBinaryString(proverCir->Rj[2]));

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

  EXPECT_EQ(Lj.length(), rangeProofCount);

  // P+V: create circuit constrains
  verifierCir->wireUp(ljir1, Lj);
  proverCir->wireUp(ljir1, Lj);

  // P: assign circuit values
  proverCir->run(ljir1, Lj);

  auto encCir = make_shared<CEnc>(crypto);
  encCir->wireUp();
  auto encCirN = encCir->gateCount;

  EXPECT_EQ(verifierCir->gateCount, encCirN * (msgCount + rangeProofCount + verifierCir->batchCount) + (msgCount * verifierCir->slotsPerMsg));
  EXPECT_EQ(verifierCir->gateCount, verifierCir->estimateGateCount());
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
      m, n, Q);
  auto proverZkp = make_shared<CircuitZKPVerifier>(
      GP_Q, GP_P, GP_G,
      proverCir->Wqa, proverCir->Wqb, proverCir->Wqc, proverCir->Kq,
      m, n, Q);
  auto prover = make_shared<CircuitZKPProver>(proverZkp, proverCir->A, proverCir->B, proverCir->C);

  // P->V: prover commit the circuit arguments
  Vec<ZZ_p> commits;
  prover->commit(commits);

  // V->P: verifier calculate challenge value Y
  verifier->setCommits(commits);
  auto y = verifier->calculateY();

  // P->V: prover perform polyComit
  Vec<ZZ_p> pc;
  prover->polyCommit(y, pc);

  // V->P: verifier calculate challenge value X
  verifier->setPolyCommits(pc);
  auto x = verifier->calculateX();

  // P->V: prover calculate the ZKP
  Vec<ZZ_p> proofs;
  prover->prove(y, x, proofs);

  // V: verify the proof
  auto isValid = verifier->verify(proofs, y, x);

  EXPECT_TRUE(isValid);
}

} // namespace
