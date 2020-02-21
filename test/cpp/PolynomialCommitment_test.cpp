#include "gtest/gtest.h"

#include "app/PolynomialCommitment.hpp"
#include "lib/paillier/PaillierEncryption.hpp"
#include "lib/math/Random.hpp"
#include "lib/math/IntegerImpl.hpp"

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
  vector<shared_ptr<Integer>> pc;
  commitScheme.commit(m1, m2, n, T, ri, pc);

  auto x = Integer::create("9", 10);

  vector<shared_ptr<Integer>> pe;
  commitScheme.eval(m1, m2, n, T, ri, x, pe);

  auto isValid = commitScheme.verify(m1, m2, n, pc, pe, x);

  EXPECT_EQ(isValid, true);

  auto v = commitScheme.calcV(n, pe, x);
  EXPECT_EQ(v->toString(), "98");
}

TEST(PolynomialCommitment, PolyCommit_eval_verify2)
{
  int byteLength = 32;
  auto crypto = PaillierEncryption::generate(byteLength);
  auto Q = crypto->getGroupQ();
  auto p = crypto->getGroupP();
  auto g = crypto->getGroupG();

  // Commitment scheme can support max 10 element as a row
  size_t n = 10;
  PolynomialCommitment commitScheme(Q, p, g, n);

  size_t cnt = 50;    // total polynomial elements count
  size_t m = cnt / n; // m = 5, compress to a matrix which 5 x 10 matrix
  size_t m1 = 3;      // -ve degree -15 to -1 (-n * m1)
  size_t m2 = m - m1; // +ve degree 1 to 35 (n * m2)

  // prepare the polynomial from x^-15, x^-14, ... , x^-1, x, x^2, ... , x^35
  vector<shared_ptr<Integer>> poly = Random::getRandoms(cnt, p);

  // prepare m + 1 randomness for commitment
  vector<shared_ptr<Integer>> ri = Random::getRandoms(m + 1, p);

  // calculate PolyCommit
  auto T = commitScheme.calcT(m1, m2, n, poly);
  vector<shared_ptr<Integer>> pc;
  commitScheme.commit(m1, m2, n, T, ri, pc);

  // set challenge value
  auto x = Random::genInteger(p);

  // calculate PolyEval
  vector<shared_ptr<Integer>> pe;
  commitScheme.eval(m1, m2, n, T, ri, x, pe);

  // verify
  auto isValid = commitScheme.verify(m1, m2, n, pc, pe, x);
  EXPECT_EQ(isValid, true);

  // calculate value v
  auto v = commitScheme.calcV(n, pe, x);
  EXPECT_TRUE(v->gt(Integer::ZERO()));
}

TEST(PolynomialCommitment, PolyCommit_eval_verify3)
{
  // Predefined group element
  auto Q = make_shared<IntegerImpl>("163D773422B7A657B68B1D3CDCC41F0A2D82D64D70E39AACF1783F882076523B7F74627EA9B6C7C639586F0920C7D4A24D84B668E425E9A60E4C98A9FFCC3C059D084491BA159EB2325BFB526B6480574510EC11C784C2678643C3668147A6088C988524169F5B558A0FFF66B3B8AFB40EDC25F18A3BA395609002DE1385F555276AC7103538EDBD820F4D48617B43FD30D14C1983B71C664FDD5DBBBE929C6EDAB4E5FDA0AC79E3D6D2678611AD809B4FC802EFDBE3D8D9FEB4AA8A6B0FEB4FE4F101929323C09D6914CDAFDFC76325695B14AAC1AF89D347B3A72DB369E761868D35EDF6E4605729D500D5753D8E19D9C29090021A5B78A0B5729B0589904D8842209DA364972FD7DCE1820AC2B32D29692B102367D977422302CACF4AE7281F292A53653AACB17AE65E88BE82A8CACEA079F6388C6A7842FD3D60B7EEBBC23AEBC91A0E3F5288B9E46B74F88F9B3374569386E1326E87B982381A5FAEA3CDBFB9CF756C1CABD269933210C810FDF0CB5B77EBEBB25CAEE92CF0A014BF9B3FDAB84689BF82544239D2126667432B73B9CCD2FDBA9D471C9153EBEC7F91E0E3DD462B3071A84FFDDC776BBE0F7AF1867835189D1110AB08DFA398949C544A5C8692C5ED17D1FFA00180D9C76438B29A6DC621607D2E45674E454984EE95B3795716BBA7A566A5BB230088E138126FB41D6BDE5A8BB19DAAF745F21A8D841B57", 16);
  auto p = make_shared<IntegerImpl>("0E05F7EE6E2AA7F0AFA21F0BE4BD3D300884F10EC7CFD449BCD404BFC3C268D105F62767D3AE8588569A202E1C3C6CDA5BC165252224843137AD24FD7A3297580837A8154426DA9B3CC232040C3A53724C5655CC280D16F459B42F8B09A2754D45BC3ABAB2348F57FC41932B51CB779DAC0CB44CA2D3A26ACCA6785995C4B3EA0732EF03EA123DA11BC81276C82F75461C4264F3154246DF6895AB56DA87523E54F02C21D080EE4684EE998D48F08D9FB93DCCE2E7F8564A6697C00691B1C308C2CCEE134756DBD0F7DAAC9234D379C96700AE733C55766E857CA0E696F5D9C6F3BCAA33AE97913D43FE1C450490BE7CC0CB63FAF5F9EB7D3E3D71E11DF96B6211D16B94946CD35970266350BC618B760369681443B948CE599F88B252C914E81DBCA2E29D26420374964D434E824EAFCCB0DA24B5F121159A79D8839B121E1D2669CBBAACECA80F97431A23EC2D2696FC6E13E1070F67BE4001663676576B117F3252E3E453A79CA2696C7E99E4E1B2504DDA1E374BE4AFDD6E4E94DBE687B94998C3CEA8ACF9DB922C2EE98E0990ADDB4D721F5B2F773E26AB753B95CC380C867A6E76D2628FC21E8F65FC413FA629E96E67CBB501AEAE94948D9B7F93BADE483E47D5CFF72BE6C832FAD0F4CA369B6238D4BAF2DF61FD0A4263528EDF05F9474EA63ED4BED4F41B1D5791CB1473D53214182F011169B8BA2EA03918EAA1", 16);
  auto g = make_shared<IntegerImpl>("109BCA88C471F06B21AF86071D58D625FEA2CEAF80C8A1132D1888067154716DA03967E375E00ADADDA9763196DB65A91ADCD3ADB1583CD4B450B3503FF528737139A9FE570C22F09348C040514C7FA61C1C3595357A4208B2495F993FEA4D826A6D673E004A91379EBE727DB415C09E334BED068EB4080B8BE18B262140D7EE0DB12706E04E2E00DCC925C9D695F78805B50CD823C070C665B07A55701409059091802D90C2463F162B781D1AB5F01C2AC86674E5689362B541E80A7256435EEC857D1A2A28FC6484D545F05703EA723090E99EA20F1844AB021735087DACD03A710BB3824AC5E81E238683887CBBECD1EA84D2BA2D4CF9415842CD47C7FCE59F2115FE67CFCC27C013CBB66D47C69C5595C5639ADB4F40C661C1B6E4821ACB93B558C9AB7C39E750A72AD3B7A54651DB933B986745A50CD97570F17068D98A17D286C898E03C48A8FD5A1D06A7A6254BD00916A7976BFB25E7F36CF92D583E5F808A200D10E2111B72DAF00880B7C3A26C9AA795AD69CF56BF0B6E7A1B6E57531734D2349D51EF59652BBE3E3E957AA4EB4C720147F951E945C51CD41743D1D9787690FB1738184830158389CCA2DF35143C87AA52E281957FE8EFCFDAC5AFEBD415D2F289CF5A2803F8565DE971095EAA26BCDC2468DAA4E0D55D0DEECE8C162D91F70F9EC4EC1323AC50EAACB828D96A3AAA659BE09D7E5F133B3C0B93FD", 16);

  // Commitment scheme can support max 10 element as a row
  size_t n = 10;
  PolynomialCommitment commitScheme(Q, p, g, n);

  size_t cnt = 50;    // total polynomial elements count
  size_t m = cnt / n; // m = 5, compress to a matrix which 5 x 10 matrix
  size_t m1 = 3;      // -ve degree -15 to -1 (-n * m1)
  size_t m2 = m - m1; // +ve degree 1 to 35 (n * m2)

  // prepare the polynomial from x^-15, x^-14, ... , x^-1, x, x^2, ... , x^35
  vector<shared_ptr<Integer>> poly = Random::getRandoms(cnt, p);

  // prepare m + 1 randomness for commitment
  vector<shared_ptr<Integer>> ri = Random::getRandoms(m + 1, p);

  // calculate PolyCommit
  auto T = commitScheme.calcT(m1, m2, n, poly);
  vector<shared_ptr<Integer>> pc;
  commitScheme.commit(m1, m2, n, T, ri, pc);

  // set challenge value
  auto x = Random::genInteger(p, {}, true);

  // calculate PolyEval
  vector<shared_ptr<Integer>> pe;
  commitScheme.eval(m1, m2, n, T, ri, x, pe);

  // verify
  auto isValid = commitScheme.verify(m1, m2, n, pc, pe, x);
  EXPECT_EQ(isValid, true);

  // calculate value v
  auto v = commitScheme.calcV(n, pe, x);
  EXPECT_TRUE(v->gt(Integer::ZERO()));
}

} // namespace
