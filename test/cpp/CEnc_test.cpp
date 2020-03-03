#include "gtest/gtest.h"

#include "lib/namespace.hpp"

#include "app/CBase.hpp"
#include "app/CEnc.hpp"
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
/*
// auto pk = make_shared<IntegerImpl>("03BEA95A1B88A128BC2EC72E7135793C58CD9C66D7ED637BF6E7BB49D1B888997FDF19F347DF1C973A1B9B7E86162CD565514D2D9A99B92C488BB690A4031BFC9727B17857C0A05D32DAB1DAA7F570B3E7BE863F5AAD6583D79A8446ADB7190E7CD74DBDDF5B38EDC2C2B4307068AD717B024F3F317E442C7812EE49FB02711DFC9FB282F1183A6A44EAED9781C4799B6760880A5ED5AC91315A87E410B0049B5309C92ECE60C493371996EFF948B08B92A29900ABB7086DC31B5FA5893A04E466C7C142A2F8CF558B9FC4098404BC922E1230AFB572189E7D7585FFDEF9E0973B9A60417609BC71853CD080E86C9F36F2325EAE4B1FA22E88CC0CF3FFD220D1", 16);
// auto sk = make_shared<IntegerImpl>("03BEA95A1B88A128BC2EC72E7135793C58CD9C66D7ED637BF6E7BB49D1B888997FDF19F347DF1C973A1B9B7E86162CD565514D2D9A99B92C488BB690A4031BFC9727B17857C0A05D32DAB1DAA7F570B3E7BE863F5AAD6583D79A8446ADB7190E7CD74DBDDF5B38EDC2C2B4307068AD717B024F3F317E442C7812EE49FB02711DB43E61FA7C3082B0F18ECCA4CD5BF257A1057E6DD39280CAA6F84167A7A6C9CF41EB8066AA9A1C7BED617BF2480147BDA4883E9D978500BCEDA1CFC6B7BD700CF424732F230B605222C12D42BC1A65CC6702AEDD3835109DF4362913E35AB17D0FB72071A9103BAF39EA3655A98AC1FFE54EB1E5345ECE68DF13CBA8B4CDAE64", 16);
// auto GP_Q = make_shared<IntegerImpl>("163D773422B7A657B68B1D3CDCC41F0A2D82D64D70E39AACF1783F882076523B7F74627EA9B6C7C639586F0920C7D4A24D84B668E425E9A60E4C98A9FFCC3C059D084491BA159EB2325BFB526B6480574510EC11C784C2678643C3668147A6088C988524169F5B558A0FFF66B3B8AFB40EDC25F18A3BA395609002DE1385F555276AC7103538EDBD820F4D48617B43FD30D14C1983B71C664FDD5DBBBE929C6EDAB4E5FDA0AC79E3D6D2678611AD809B4FC802EFDBE3D8D9FEB4AA8A6B0FEB4FE4F101929323C09D6914CDAFDFC76325695B14AAC1AF89D347B3A72DB369E761868D35EDF6E4605729D500D5753D8E19D9C29090021A5B78A0B5729B0589904D8842209DA364972FD7DCE1820AC2B32D29692B102367D977422302CACF4AE7281F292A53653AACB17AE65E88BE82A8CACEA079F6388C6A7842FD3D60B7EEBBC23AEBC91A0E3F5288B9E46B74F88F9B3374569386E1326E87B982381A5FAEA3CDBFB9CF756C1CABD269933210C810FDF0CB5B77EBEBB25CAEE92CF0A014BF9B3FDAB84689BF82544239D2126667432B73B9CCD2FDBA9D471C9153EBEC7F91E0E3DD462B3071A84FFDDC776BBE0F7AF1867835189D1110AB08DFA398949C544A5C8692C5ED17D1FFA00180D9C76438B29A6DC621607D2E45674E454984EE95B3795716BBA7A566A5BB230088E138126FB41D6BDE5A8BB19DAAF745F21A8D841B57");
// auto GP_P = make_shared<IntegerImpl>("0E05F7EE6E2AA7F0AFA21F0BE4BD3D300884F10EC7CFD449BCD404BFC3C268D105F62767D3AE8588569A202E1C3C6CDA5BC165252224843137AD24FD7A3297580837A8154426DA9B3CC232040C3A53724C5655CC280D16F459B42F8B09A2754D45BC3ABAB2348F57FC41932B51CB779DAC0CB44CA2D3A26ACCA6785995C4B3EA0732EF03EA123DA11BC81276C82F75461C4264F3154246DF6895AB56DA87523E54F02C21D080EE4684EE998D48F08D9FB93DCCE2E7F8564A6697C00691B1C308C2CCEE134756DBD0F7DAAC9234D379C96700AE733C55766E857CA0E696F5D9C6F3BCAA33AE97913D43FE1C450490BE7CC0CB63FAF5F9EB7D3E3D71E11DF96B6211D16B94946CD35970266350BC618B760369681443B948CE599F88B252C914E81DBCA2E29D26420374964D434E824EAFCCB0DA24B5F121159A79D8839B121E1D2669CBBAACECA80F97431A23EC2D2696FC6E13E1070F67BE4001663676576B117F3252E3E453A79CA2696C7E99E4E1B2504DDA1E374BE4AFDD6E4E94DBE687B94998C3CEA8ACF9DB922C2EE98E0990ADDB4D721F5B2F773E26AB753B95CC380C867A6E76D2628FC21E8F65FC413FA629E96E67CBB501AEAE94948D9B7F93BADE483E47D5CFF72BE6C832FAD0F4CA369B6238D4BAF2DF61FD0A4263528EDF05F9474EA63ED4BED4F41B1D5791CB1473D53214182F011169B8BA2EA03918EAA1");
// auto GP_G = make_shared<IntegerImpl>("109BCA88C471F06B21AF86071D58D625FEA2CEAF80C8A1132D1888067154716DA03967E375E00ADADDA9763196DB65A91ADCD3ADB1583CD4B450B3503FF528737139A9FE570C22F09348C040514C7FA61C1C3595357A4208B2495F993FEA4D826A6D673E004A91379EBE727DB415C09E334BED068EB4080B8BE18B262140D7EE0DB12706E04E2E00DCC925C9D695F78805B50CD823C070C665B07A55701409059091802D90C2463F162B781D1AB5F01C2AC86674E5689362B541E80A7256435EEC857D1A2A28FC6484D545F05703EA723090E99EA20F1844AB021735087DACD03A710BB3824AC5E81E238683887CBBECD1EA84D2BA2D4CF9415842CD47C7FCE59F2115FE67CFCC27C013CBB66D47C69C5595C5639ADB4F40C661C1B6E4821ACB93B558C9AB7C39E750A72AD3B7A54651DB933B986745A50CD97570F17068D98A17D286C898E03C48A8FD5A1D06A7A6254BD00916A7976BFB25E7F36CF92D583E5F808A200D10E2111B72DAF00880B7C3A26C9AA795AD69CF56BF0B6E7A1B6E57531734D2349D51EF59652BBE3E3E957AA4EB4C720147F951E945C51CD41743D1D9787690FB1738184830158389CCA2DF35143C87AA52E281957FE8EFCFDAC5AFEBD415D2F289CF5A2803F8565DE971095EAA26BCDC2468DAA4E0D55D0DEECE8C162D91F70F9EC4EC1323AC50EAACB828D96A3AAA659BE09D7E5F133B3C0B93FD");
/*/
int byteLength = 16;
auto crypto = PaillierEncryption::generate(byteLength);
auto GP_Q = crypto -> getGroupQ();
auto GP_P = crypto -> getGroupP();
auto GP_G = crypto -> getGroupG();
auto pk = crypto -> getPublicKey();
auto sk = crypto -> getPrivateKey();
//*/

auto decryptor = make_shared<PaillierEncryption>(pk, sk, GP_Q, GP_P, GP_G);
auto encryptor = make_shared<PaillierEncryption>(pk, GP_Q, GP_P, GP_G);

TEST(CEnc, Create_circuit)
{
  auto pk = make_shared<IntegerImpl>(101);
  auto c = make_shared<IntegerImpl>(9);
  auto crypto = make_shared<PaillierEncryption>(pk, GP_Q, GP_P, GP_G);
  auto circuit = make_shared<CEnc>(crypto);
  circuit->wireUp(c);

  EXPECT_EQ(circuit->Wqa.size(), 23);
  EXPECT_EQ(circuit->Wqb.size(), 23);
  EXPECT_EQ(circuit->Wqc.size(), 23);
  EXPECT_EQ(circuit->Kq.size(), 23);
  EXPECT_EQ(circuit->A->n, 12);
  EXPECT_EQ(circuit->B->n, 12);
  EXPECT_EQ(circuit->C->n, 12);

  EXPECT_EQ(circuit->N->toString(), "101");
  EXPECT_EQ(circuit->Wqa[22]->m, 1);
  EXPECT_EQ(circuit->Wqa[22]->n, 12); // N = 11
  EXPECT_EQ(circuit->Kq[0]->toString(), "101");
  EXPECT_EQ(circuit->Kq[22]->toString(), "9");
}

TEST(CEnc, Run_circuit)
{
  cout << "\"byteLength\"," << byteLength << endl;
  cout << "\"p=N^2\"," << GP_P->toBinary().size() * 8 << endl;
  cout << "\"Q\"," << GP_Q->toBinary().size() * 8 << endl;
  cout << "\"G\"," << GP_G->toBinary().size() * 8 << endl;
  cout << "\"pk=N\"," << pk->toBinary().size() * 8 << endl;
  cout << "\"sk=lamda\"," << sk->toBinary().size() * 8 << endl;

  double totalCreate = 0;
  double totalValueAsign = 0;
  double totalCommit = 0;
  double totalChallenge = 0;
  double totalProve = 0;
  double totalVerify = 0;

  auto msg = make_shared<IntegerImpl>(123);
  auto rand = make_shared<IntegerImpl>(456);
  auto c = encryptor->encrypt(msg, rand);

  Timer::start("circuit.create");
  auto circuit = make_shared<CEnc>(crypto);
  circuit->wireUp(c);
  totalCreate += Timer::end("circuit.create");

  Timer::start("circuit.run");
  circuit->run(msg, rand);
  totalValueAsign += Timer::end("circuit.run");

  EXPECT_EQ(circuit->Kq[0]->toString(), encryptor->getPublicKey()->toString());
  EXPECT_EQ(circuit->Kq[circuit->linearCount - 1]->toString(), c->toString());
  EXPECT_EQ(circuit->C->cell(0, circuit->gateCount - 1)->toString(), c->toString());

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
      m, n);
  auto prover = make_shared<CircuitZKPProver>(verifier, circuit->A, circuit->B, circuit->C);

  Timer::start("commit");
  vector<shared_ptr<Integer>> commits;
  prover->commit(commits);
  totalCommit += Timer::end("commit");

  Timer::start("calculateY");
  verifier->setCommits(commits);
  auto y = verifier->calculateY();
  totalChallenge += Timer::end("calculateY");

  Timer::start("polyCommit");
  vector<shared_ptr<Integer>> pc;
  prover->polyCommit(y, pc);
  totalProve += Timer::end("polyCommit");

  Timer::start("calculateX");
  verifier->setPolyCommits(pc);
  auto x = verifier->calculateX();
  totalChallenge += Timer::end("calculateX");

  Timer::start("prove");
  vector<shared_ptr<Integer>> proofs;
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
      auto crypto = PaillierEncryption::generate(byteLength);
      auto GP_Q = crypto->getGroupQ();
      auto GP_P = crypto->getGroupP();
      auto GP_G = crypto->getGroupG();
      auto pk = crypto->getPublicKey();
      auto sk = crypto->getPrivateKey();
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
