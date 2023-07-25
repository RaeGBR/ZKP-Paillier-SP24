#include "./EndToEnd.hpp"

// for end to end implementation


void polyu::end_to_end(size_t byteLength, size_t msgCount, size_t rangeProofCount, size_t slotSize, size_t msgPerBatch, ofstream &fs)
{
  auto crypto = make_shared<PaillierEncryption>(byteLength);
  polyu::end_to_end(crypto, msgCount, rangeProofCount, slotSize, msgPerBatch, fs);
}

void polyu::end_to_end(const shared_ptr<PaillierEncryption> &crypto, size_t msgCount, size_t rangeProofCount, size_t slotSize, size_t msgPerBatch, ofstream &fs)
{
  // extract system parameters from private keys
  auto GP_Q = crypto->getGroupQ(); // public parameter: group element Q
  auto GP_P = crypto->getGroupP(); // public parameter: group order p
  ZZ_p::init(GP_Q);
  auto GP_G = crypto->getGroupG();         // public parameter: group generator g
  auto pk = crypto->getPublicKey();        // public key
  auto sk1 = crypto->getPrivateElement1(); // private key component
  auto sk2 = crypto->getPrivateElement2(); // private key component

  // variables for benchmark test
  auto byteLength = NumBytes(pk);
  ZZ_p::init(GP_P);

  double encryptTime = 0;
  double circuitTime = 0;
  double valueTime = 0;
  double commitTime = 0;
  double proveTime = 0;
  double verifyTime = 0;

  // for end-to-end time
  double phase1Time = 0;
  double phase2Time = 0;
  double attachTime = 0; // verify attach weights
  double decomposeTime = 0;

  double amoCmtTime = 0; // cmt_time / bits_num
  double amoTotalProofTime = 0; // total_proof_time / bits_num
  double amotVrfTime = 0; // vrf_time / bits_num
  double amoProofCost = 0; // bits 

  auto encCir = make_shared<CEnc>(crypto);
  encCir->wireUp();
  auto encCirN = encCir->gateCount;
  auto encCirQ = encCir->linearCount;

  /*
   * Prover's flow
   */
  auto decryptor = make_shared<PaillierEncryption>(pk, sk1, sk2, GP_Q, GP_P, GP_G);
  auto proverCir = make_shared<CBatchEnc>(decryptor, msgCount, rangeProofCount, slotSize, msgPerBatch);
  auto giRequired = proverCir->estimateGeneratorsRequired();
  auto gi = decryptor->genGenerators(giRequired); // public paramters: generators gi for commitment scheme

  cout << "====================" << endl;
  // P: prover prepare structured message
  Vec<ZZ> msg;
  for (size_t i = 0; i < msgCount; i++)
  {
    ZZ max = conv<ZZ>(2) << (proverCir->slotsPerMsg - 1);
    ZZ q;
    ZZ r;
    DivRem(q, r, conv<ZZ>(i), max);
    auto rStr = ConvertUtils::toBinaryString(r);

    ZZ m;
    for (size_t i = 0; i < rStr.size(); i++)
    {
      m = m << (proverCir->slotSize * 8);
      m += rStr[i] - '0';
    }
    msg.append(m);
  }

  // P: prover batch encrypt message
  Timer::start("P.encrypt");
  proverCir->encrypt(msg);
  encryptTime += Timer::end("P.encrypt");

  // P: prover calculate challenge value Ljir (non-interactive mode)
  Timer::start("P.Ljir");
  auto ljir1 = proverCir->calculateLjir();
  circuitTime += Timer::end("P.Ljir");

  // P: prover compute Lj for range proof
  Timer::start("P.Lj");
  auto Lj = proverCir->calculateLj(ljir1);
  circuitTime += Timer::end("P.Lj");

  // P: create circuit constrains
  Timer::start("P.wireUp");
  proverCir->wireUp(ljir1, Lj);
  circuitTime += Timer::end("P.wireUp");

  // P: assign circuit values
  Timer::start("P.run");
  proverCir->run(ljir1, Lj);
  valueTime += Timer::end("P.run");

  // P: setup ZKP protocol for the circuit
  Timer::start("P.circuit");
  auto prover = proverCir->generateProver(gi);
  circuitTime += Timer::end("P.circuit");

  // Prover need to calculate the following values to verifier
  // P->V: Cm, Cm', CR'j, Lj (circuit wire up)
  //       commits           (commits of A, B, C, D)
  //       pc                (commits of polynomial t(X))
  //       proofs            (pe, r, rr)

  // V: verifier receive cipher and calculate challenge (lj)
  auto Cm = proverCir->Cm;
  auto Cm_ = proverCir->Cm_;
  auto CRj = proverCir->CRj;
  auto Rm = proverCir->Rm;

  // benchmark variables
  auto batchCount = proverCir->batchCount;
  auto msgSize = proverCir->msgSize;
  auto slotsPerMsg = proverCir->slotsPerMsg;
  auto batchCirN = proverCir->gateCount;
  auto batchCirQ = proverCir->linearCount;
  cout << "message count: " << msgCount << endl;
  cout << "byte length: " << byteLength * 8 << "-bit" << endl;
  cout << "single encrypt circuit's N: " << encCirN << endl;
  cout << "single encrypt circuit's Q: " << encCirQ << endl;
  cout << "batch encrypt circuit's N: " << batchCirN << endl;
  cout << "batch encrypt circuit's Q: " << batchCirQ << endl;

  proverCir = nullptr; // clean up, save memory

  // P: prover commit the circuit arguments
  Timer::start("P.commit");
  Vec<ZZ_p> commits;
  prover->commit(commits);
  commitTime += Timer::end("P.commit");

  // P: prover calculate challenge value Y (non-interactive mode)
  Timer::start("P.y");
  prover->zkp->setCommits(commits);
  auto y1 = prover->zkp->calculateY();
  proveTime += Timer::end("P.y");

  // P: prover perform polyCommit
  Timer::start("P.polyCommit");
  Vec<ZZ_p> pc;
  prover->polyCommit(y1, pc);
  proveTime += Timer::end("P.polyCommit");

  // P: prover calculate challenge value X (non-interactive mode)
  Timer::start("P.x");
  prover->zkp->setPolyCommits(pc);
  auto x1 = prover->zkp->calculateX();
  proveTime += Timer::end("P.x");

  // P: prover calculate the ZKP
  Timer::start("P.prove");
  Vec<ZZ_p> proofs;
  prover->prove(y1, x1, proofs);
  proveTime += Timer::end("P.prove");

  prover = nullptr; // clean up, save memory


  /*
   * Verifier
   */

  // Verifier received the following values and perfrom the verify protocol
  // P->V: Cm, Cm', CR'j, Lj (circuit wire up)
  //       commits           (commits of A, B, C, D)
  //       pc                (commits of polynomial t(X))
  //       proofs            (pe, r, rr)
  auto encryptor = make_shared<PaillierEncryption>(pk, GP_Q, GP_P, GP_G);
  auto verifierCir = make_shared<CBatchEnc>(encryptor, msgCount, rangeProofCount, slotSize, msgPerBatch);

  // V: verifier receive cipher and calculate challenge value Ljir (non-interactive mode)
  Timer::start("V.Ljir");
  verifierCir->setCipher(Cm, Cm_, CRj);
  auto ljir2 = verifierCir->calculateLjir();
  circuitTime += Timer::end("V.Ljir");

  // V: create circuit constrains
  Timer::start("V.wireUp");
  verifierCir->wireUp(ljir2, Lj);
  circuitTime += Timer::end("V.wireUp");

  // V: setup ZKP protocol for the circuit
  Timer::start("V.circuit");
  auto verifier = verifierCir->generateVerifier(gi);
  verifierCir = nullptr; // clean up, save memory
  circuitTime += Timer::end("V.circuit");

  // V: verifier calculate challenge value Y (non-interactive mode)
  Timer::start("V.y");
  verifier->setCommits(commits);
  auto y = verifier->calculateY();
  verifyTime += Timer::end("V.y");

  // V: verifier calculate challenge value X (non-interactive mode)
  Timer::start("V.x");
  verifier->setPolyCommits(pc);
  auto x = verifier->calculateX();
  verifyTime += Timer::end("V.x");

  // V: verify the proof
  Timer::start("V.verify");
  auto isValid = verifier->verify(proofs, y, x);
  verifyTime += Timer::end("V.verify");

  // todo: 1. Verifier: attach Wi
  // 2. Verifier: calculate the result 
  // 3. Prover: decypt
  // 4. Prover: decompose and calculate the ave.

  // 1.2. V: attach wi and calculate result
  ZZ max = power(conv<ZZ>(2), slotSize * 8) - 1;
  max = max / conv<ZZ>(msgCount);
  long num_bits = NumBits(max) - 1;
  // cout << "max  is " << max << endl;
  // cout << "its num bits is " <<s num_bits << endl;
  
  ZZ_p result = conv<ZZ_p>(1);
  ZZ sum = conv<ZZ>(0);

  Timer::start("V.attach_value");
  for (size_t i = 0; i < Cm.length(); i++) {
    // random weight wi
    auto random_wi = RandomBits_ZZ(num_bits);
    auto c_wi = crypto->mul(Cm[i], random_wi);

    ZZ_p::init(GP_P);
    result *= c_wi;
    sum += random_wi * msg[i];
}
  attachTime += Timer::end("V.attach_value");
  
// 3. P: decrypt

ZZ sum_m = crypto->decrypt(result);
//todo: to be deleted
// cout << "decrypt final result is " << sum_m << endl;
// cout << "should be " << sum << endl;

// 4. P: decopmpose and calculate the average
string m_decmp = ConvertUtils::toBinaryString(sum_m);
size_t slot_num = byteLength / slotSize;
vector<size_t> ave_result;

Timer::start("P.decompose");
for (size_t i = 0; i < slot_num; i++) {
    size_t agg_result = 0;

    for (size_t j = 0; j < slotSize * 8; j++) {
        agg_result += static_cast<size_t>(m_decmp[i * slot_num * 8 + j]);
    }
    // cout << i << "-th slot aggregation result is" << agg_result << endl;
    ave_result.push_back(agg_result/msgCount);
}
decomposeTime += Timer::endNan("P.decompose");


// output the average for every attribute
// for (size_t i = 0; i < slot_num; i++) {
//     cout << "the " << i << "-th slot's averaged result is " << ave_result[i] << endl;
// }


  // print out benchmark result
  circuitTime /= 2;

  const auto m = verifier->m;
  const auto n = verifier->n;

  auto pSize = NumBytes(GP_P);
  auto qSize = NumBytes(GP_Q);
  double cipherSize = pSize;
  double proofSize = 1.0 * pSize * (batchCount + rangeProofCount); // Cm', CRj
  proofSize += pSize * Lj.length();
  proofSize += qSize * commits.length();
  proofSize += qSize * pc.length();
  proofSize += pSize * proofs.length();
  double proofSizePerMsg = proofSize / msgCount;

  phase1Time = encryptTime + circuitTime + valueTime + commitTime + proveTime; // total proof time
  phase2Time = verifyTime + attachTime; 

  amoCmtTime = commitTime * 1000 / (msgCount * slotsPerMsg); // ms
  amoTotalProofTime = phase1Time * 1000 / (msgCount * slotsPerMsg); //ms
  amotVrfTime = verifyTime * 1000 / (msgCount * slotsPerMsg); // ms 

  amoProofCost = proofSizePerMsg * 8 / slotsPerMsg; // bits
 

  cout << endl;
  cout << "==========" << endl;
  cout << "message count: " << msgCount << endl;
  cout << "message/batch: " << msgPerBatch << endl;
  cout << "batch count: " << batchCount << endl;
  cout << "byte length: " << byteLength * 8 << "-bit" << endl;
  cout << "byte length: " << byteLength << " bytes" << endl;
  cout << "message size: " << msgSize << " bytes" << endl;
  cout << "slot size: " << slotSize << " bytes" << endl;
  cout << "slot/message: " << slotsPerMsg << endl;
  cout << "range proof count: " << rangeProofCount << endl;
  cout << "cipher size: " << cipherSize << " bytes" << endl;
  cout << "proof size: " << proofSize << " bytes" << endl;
  cout << "proof size/message: " << proofSizePerMsg << " bytes" << endl;

  cout << endl;
  cout << "single encrypt circuit's N: " << encCirN << endl;
  cout << "single encrypt circuit's Q: " << encCirQ << endl;
  cout << "batch encrypt circuit's N: " << batchCirN << endl;
  cout << "batch encrypt circuit's Q: " << batchCirQ << endl;
  cout << "batch encrypt circuit's matrix m: " << m << endl;
  cout << "batch encrypt circuit's matrix n: " << n << endl;

  cout << endl;
  cout << "encryption time: " << encryptTime << endl;
  cout << "circuit create time: " << circuitTime << endl;
  cout << "value assign time: " << valueTime << endl;
  cout << "commit time: " << commitTime << endl;
  cout << "prove time: " << proveTime << endl;
  cout << "verify time: " << verifyTime << endl;
  cout << "==========" << endl;

  fs << msgCount << ",";
  fs << msgPerBatch << ",";
  fs << batchCount << ",";
  fs << byteLength * 8 << ",";
  fs << byteLength << ",";
  fs << msgSize << ",";
  fs << slotSize << ",";
  fs << slotsPerMsg << ",";
  fs << rangeProofCount << ",";
  fs << cipherSize << ",";
  fs << proofSize << ",";
  fs << proofSizePerMsg << ",";
  fs << encCirN << ",";
  fs << encCirQ << ",";
  fs << batchCirN << ",";
  fs << batchCirQ << ",";
  fs << m << ",";
  fs << n << ",";
  fs << encryptTime << ",";
  fs << circuitTime << ",";
  fs << valueTime << ",";
  fs << commitTime << ",";
  fs << proveTime << ",";
  fs << verifyTime << ",";

  fs << attachTime << ","; // for end-to-end
  fs << phase1Time << ",";
  fs << phase2Time << ",";
  fs << double(decomposeTime/1000000) << ","; // ms
  fs << decomposeTime << ","; // ns

  // for amortized time 
  fs << amoCmtTime << ",";
  fs << amoTotalProofTime << ",";
  fs << amotVrfTime << ",";
  fs << amoProofCost << endl;

  if (!isValid)
    throw invalid_argument("zkp is not valid");
}

// ZZ_p polyu:: help_random (ZZ max) {
//     ZZ_pPush push(max);
//     ZZ_p r;
//     while (true) {
//         random(r);
//         if (r == 0) 
//             continue;
//         return r;
//     }
// }
