#include "gtest/gtest.h"

#include "lib/namespace.hpp"

#include "app/CBase.hpp"
#include "app/CEnc.hpp"
#include "lib/math/IntegerImpl.hpp"
#include "lib/math/Matrix.hpp"

using namespace cryptoplus;
using namespace polyu;

namespace
{

auto pk = make_shared<IntegerImpl>("03BEA95A1B88A128BC2EC72E7135793C58CD9C66D7ED637BF6E7BB49D1B888997FDF19F347DF1C973A1B9B7E86162CD565514D2D9A99B92C488BB690A4031BFC9727B17857C0A05D32DAB1DAA7F570B3E7BE863F5AAD6583D79A8446ADB7190E7CD74DBDDF5B38EDC2C2B4307068AD717B024F3F317E442C7812EE49FB02711DFC9FB282F1183A6A44EAED9781C4799B6760880A5ED5AC91315A87E410B0049B5309C92ECE60C493371996EFF948B08B92A29900ABB7086DC31B5FA5893A04E466C7C142A2F8CF558B9FC4098404BC922E1230AFB572189E7D7585FFDEF9E0973B9A60417609BC71853CD080E86C9F36F2325EAE4B1FA22E88CC0CF3FFD220D1", 16);
auto sk = make_shared<IntegerImpl>("03BEA95A1B88A128BC2EC72E7135793C58CD9C66D7ED637BF6E7BB49D1B888997FDF19F347DF1C973A1B9B7E86162CD565514D2D9A99B92C488BB690A4031BFC9727B17857C0A05D32DAB1DAA7F570B3E7BE863F5AAD6583D79A8446ADB7190E7CD74DBDDF5B38EDC2C2B4307068AD717B024F3F317E442C7812EE49FB02711DB43E61FA7C3082B0F18ECCA4CD5BF257A1057E6DD39280CAA6F84167A7A6C9CF41EB8066AA9A1C7BED617BF2480147BDA4883E9D978500BCEDA1CFC6B7BD700CF424732F230B605222C12D42BC1A65CC6702AEDD3835109DF4362913E35AB17D0FB72071A9103BAF39EA3655A98AC1FFE54EB1E5345ECE68DF13CBA8B4CDAE64", 16);
auto GP_Q = make_shared<IntegerImpl>("163D773422B7A657B68B1D3CDCC41F0A2D82D64D70E39AACF1783F882076523B7F74627EA9B6C7C639586F0920C7D4A24D84B668E425E9A60E4C98A9FFCC3C059D084491BA159EB2325BFB526B6480574510EC11C784C2678643C3668147A6088C988524169F5B558A0FFF66B3B8AFB40EDC25F18A3BA395609002DE1385F555276AC7103538EDBD820F4D48617B43FD30D14C1983B71C664FDD5DBBBE929C6EDAB4E5FDA0AC79E3D6D2678611AD809B4FC802EFDBE3D8D9FEB4AA8A6B0FEB4FE4F101929323C09D6914CDAFDFC76325695B14AAC1AF89D347B3A72DB369E761868D35EDF6E4605729D500D5753D8E19D9C29090021A5B78A0B5729B0589904D8842209DA364972FD7DCE1820AC2B32D29692B102367D977422302CACF4AE7281F292A53653AACB17AE65E88BE82A8CACEA079F6388C6A7842FD3D60B7EEBBC23AEBC91A0E3F5288B9E46B74F88F9B3374569386E1326E87B982381A5FAEA3CDBFB9CF756C1CABD269933210C810FDF0CB5B77EBEBB25CAEE92CF0A014BF9B3FDAB84689BF82544239D2126667432B73B9CCD2FDBA9D471C9153EBEC7F91E0E3DD462B3071A84FFDDC776BBE0F7AF1867835189D1110AB08DFA398949C544A5C8692C5ED17D1FFA00180D9C76438B29A6DC621607D2E45674E454984EE95B3795716BBA7A566A5BB230088E138126FB41D6BDE5A8BB19DAAF745F21A8D841B57");
auto GP_P = make_shared<IntegerImpl>("0E05F7EE6E2AA7F0AFA21F0BE4BD3D300884F10EC7CFD449BCD404BFC3C268D105F62767D3AE8588569A202E1C3C6CDA5BC165252224843137AD24FD7A3297580837A8154426DA9B3CC232040C3A53724C5655CC280D16F459B42F8B09A2754D45BC3ABAB2348F57FC41932B51CB779DAC0CB44CA2D3A26ACCA6785995C4B3EA0732EF03EA123DA11BC81276C82F75461C4264F3154246DF6895AB56DA87523E54F02C21D080EE4684EE998D48F08D9FB93DCCE2E7F8564A6697C00691B1C308C2CCEE134756DBD0F7DAAC9234D379C96700AE733C55766E857CA0E696F5D9C6F3BCAA33AE97913D43FE1C450490BE7CC0CB63FAF5F9EB7D3E3D71E11DF96B6211D16B94946CD35970266350BC618B760369681443B948CE599F88B252C914E81DBCA2E29D26420374964D434E824EAFCCB0DA24B5F121159A79D8839B121E1D2669CBBAACECA80F97431A23EC2D2696FC6E13E1070F67BE4001663676576B117F3252E3E453A79CA2696C7E99E4E1B2504DDA1E374BE4AFDD6E4E94DBE687B94998C3CEA8ACF9DB922C2EE98E0990ADDB4D721F5B2F773E26AB753B95CC380C867A6E76D2628FC21E8F65FC413FA629E96E67CBB501AEAE94948D9B7F93BADE483E47D5CFF72BE6C832FAD0F4CA369B6238D4BAF2DF61FD0A4263528EDF05F9474EA63ED4BED4F41B1D5791CB1473D53214182F011169B8BA2EA03918EAA1");
auto GP_G = make_shared<IntegerImpl>("109BCA88C471F06B21AF86071D58D625FEA2CEAF80C8A1132D1888067154716DA03967E375E00ADADDA9763196DB65A91ADCD3ADB1583CD4B450B3503FF528737139A9FE570C22F09348C040514C7FA61C1C3595357A4208B2495F993FEA4D826A6D673E004A91379EBE727DB415C09E334BED068EB4080B8BE18B262140D7EE0DB12706E04E2E00DCC925C9D695F78805B50CD823C070C665B07A55701409059091802D90C2463F162B781D1AB5F01C2AC86674E5689362B541E80A7256435EEC857D1A2A28FC6484D545F05703EA723090E99EA20F1844AB021735087DACD03A710BB3824AC5E81E238683887CBBECD1EA84D2BA2D4CF9415842CD47C7FCE59F2115FE67CFCC27C013CBB66D47C69C5595C5639ADB4F40C661C1B6E4821ACB93B558C9AB7C39E750A72AD3B7A54651DB933B986745A50CD97570F17068D98A17D286C898E03C48A8FD5A1D06A7A6254BD00916A7976BFB25E7F36CF92D583E5F808A200D10E2111B72DAF00880B7C3A26C9AA795AD69CF56BF0B6E7A1B6E57531734D2349D51EF59652BBE3E3E957AA4EB4C720147F951E945C51CD41743D1D9787690FB1738184830158389CCA2DF35143C87AA52E281957FE8EFCFDAC5AFEBD415D2F289CF5A2803F8565DE971095EAA26BCDC2468DAA4E0D55D0DEECE8C162D91F70F9EC4EC1323AC50EAACB828D96A3AAA659BE09D7E5F133B3C0B93FD");

TEST(CBase, Constructor1)
{
  size_t Q = 2;
  vector<shared_ptr<Matrix>> Wqa;
  vector<shared_ptr<Matrix>> Wqb;
  vector<shared_ptr<Matrix>> Wqc;
  vector<shared_ptr<Integer>> Kq;

  Wqa.push_back(make_shared<Matrix>(vector<int>({1, 0, 0, 0, 0, 0})));
  Wqa.push_back(make_shared<Matrix>(vector<int>({0, 1, 0, 0, 0, 0})));
  Wqb.push_back(make_shared<Matrix>(vector<int>({0, 0, 1, 0, 0, 0})));
  Wqb.push_back(make_shared<Matrix>(vector<int>({0, 0, 0, 1, 0, 0})));
  Wqc.push_back(make_shared<Matrix>(vector<int>({0, 0, 0, 0, 1, 0})));
  Wqc.push_back(make_shared<Matrix>(vector<int>({0, 0, 0, 0, 0, 1})));
  Kq.push_back(make_shared<IntegerImpl>(7));
  Kq.push_back(make_shared<IntegerImpl>(9));

  auto circuit = make_shared<CEnc>(GP_Q, GP_P, GP_G, Wqa, Wqb, Wqc, Kq);

  EXPECT_EQ(circuit->GP_Q->toString(), GP_Q->toString());
  EXPECT_EQ(circuit->GP_P->toString(), GP_P->toString());
  EXPECT_EQ(circuit->GP_G->toString(), GP_G->toString());
  EXPECT_EQ(circuit->Wqa.size(), Wqa.size());
  EXPECT_EQ(circuit->Wqb.size(), Wqb.size());
  EXPECT_EQ(circuit->Wqc.size(), Wqc.size());
  EXPECT_EQ(circuit->Kq.size(), Kq.size());
  for (size_t i = 0; i < Q; i++)
  {
    EXPECT_EQ(circuit->Wqa[i]->toString(), Wqa[i]->toString());
    EXPECT_EQ(circuit->Wqb[i]->toString(), Wqb[i]->toString());
    EXPECT_EQ(circuit->Wqc[i]->toString(), Wqc[i]->toString());
    EXPECT_EQ(circuit->Kq[i]->toString(), Kq[i]->toString());
  }
  EXPECT_EQ(circuit->A, nullptr);
  EXPECT_EQ(circuit->B, nullptr);
  EXPECT_EQ(circuit->C, nullptr);
}

TEST(CBase, Constructor2)
{
  size_t Q = 2;
  vector<shared_ptr<Matrix>> Wqa;
  vector<shared_ptr<Matrix>> Wqb;
  vector<shared_ptr<Matrix>> Wqc;
  vector<shared_ptr<Integer>> Kq;

  Wqa.push_back(make_shared<Matrix>(vector<int>({1, 0, 0, 0, 0, 0})));
  Wqa.push_back(make_shared<Matrix>(vector<int>({0, 1, 0, 0, 0, 0})));
  Wqb.push_back(make_shared<Matrix>(vector<int>({0, 0, 1, 0, 0, 0})));
  Wqb.push_back(make_shared<Matrix>(vector<int>({0, 0, 0, 1, 0, 0})));
  Wqc.push_back(make_shared<Matrix>(vector<int>({0, 0, 0, 0, 1, 0})));
  Wqc.push_back(make_shared<Matrix>(vector<int>({0, 0, 0, 0, 0, 1})));
  Kq.push_back(make_shared<IntegerImpl>(7));
  Kq.push_back(make_shared<IntegerImpl>(9));

  auto A = make_shared<Matrix>(vector<int>({1, 2, 0, 0, 0, 0}));
  auto B = make_shared<Matrix>(vector<int>({0, 0, 3, 4, 0, 0}));
  auto C = make_shared<Matrix>(vector<int>({0, 0, 0, 0, 5, 6}));

  auto circuit = make_shared<CEnc>(GP_Q, GP_P, GP_G, Wqa, Wqb, Wqc, Kq, A, B, C);

  EXPECT_EQ(circuit->GP_Q->toString(), GP_Q->toString());
  EXPECT_EQ(circuit->GP_P->toString(), GP_P->toString());
  EXPECT_EQ(circuit->GP_G->toString(), GP_G->toString());
  EXPECT_EQ(circuit->Wqa.size(), Wqa.size());
  EXPECT_EQ(circuit->Wqb.size(), Wqb.size());
  EXPECT_EQ(circuit->Wqc.size(), Wqc.size());
  EXPECT_EQ(circuit->Kq.size(), Kq.size());
  for (size_t i = 0; i < Q; i++)
  {
    EXPECT_EQ(circuit->Wqa[i]->toString(), Wqa[i]->toString());
    EXPECT_EQ(circuit->Wqb[i]->toString(), Wqb[i]->toString());
    EXPECT_EQ(circuit->Wqc[i]->toString(), Wqc[i]->toString());
    EXPECT_EQ(circuit->Kq[i]->toString(), Kq[i]->toString());
  }
  EXPECT_EQ(circuit->A->toString(), A->toString());
  EXPECT_EQ(circuit->B->toString(), B->toString());
  EXPECT_EQ(circuit->C->toString(), C->toString());
}

TEST(CBase, CopyCircuit)
{
  size_t Q = 2;
  vector<shared_ptr<Matrix>> Wqa;
  vector<shared_ptr<Matrix>> Wqb;
  vector<shared_ptr<Matrix>> Wqc;
  vector<shared_ptr<Integer>> Kq;

  Wqa.push_back(make_shared<Matrix>(vector<int>({1, 0, 0, 0, 0, 0})));
  Wqa.push_back(make_shared<Matrix>(vector<int>({0, 1, 0, 0, 0, 0})));
  Wqb.push_back(make_shared<Matrix>(vector<int>({0, 0, 1, 0, 0, 0})));
  Wqb.push_back(make_shared<Matrix>(vector<int>({0, 0, 0, 1, 0, 0})));
  Wqc.push_back(make_shared<Matrix>(vector<int>({0, 0, 0, 0, 1, 0})));
  Wqc.push_back(make_shared<Matrix>(vector<int>({0, 0, 0, 0, 0, 1})));
  Kq.push_back(make_shared<IntegerImpl>(7));
  Kq.push_back(make_shared<IntegerImpl>(9));

  auto A = make_shared<Matrix>(vector<int>({1, 2, 0, 0, 0, 0}));
  auto B = make_shared<Matrix>(vector<int>({0, 0, 3, 4, 0, 0}));
  auto C = make_shared<Matrix>(vector<int>({0, 0, 0, 0, 5, 6}));

  auto circuit1 = make_shared<CEnc>(GP_Q, GP_P, GP_G, Wqa, Wqb, Wqc, Kq, A, B, C);
  auto circuit2 = make_shared<CBase>();
  CBase::copyCircuit(circuit1, circuit2);

  EXPECT_EQ(circuit1->GP_Q->toString(), circuit2->GP_Q->toString());
  EXPECT_EQ(circuit1->GP_P->toString(), circuit2->GP_P->toString());
  EXPECT_EQ(circuit1->GP_G->toString(), circuit2->GP_G->toString());
  EXPECT_EQ(circuit1->Wqa.size(), circuit2->Wqa.size());
  EXPECT_EQ(circuit1->Wqb.size(), circuit2->Wqb.size());
  EXPECT_EQ(circuit1->Wqc.size(), circuit2->Wqc.size());
  EXPECT_EQ(circuit1->Kq.size(), circuit2->Kq.size());
  for (size_t i = 0; i < Q; i++)
  {
    EXPECT_EQ(circuit1->Wqa[i]->toString(), circuit2->Wqa[i]->toString());
    EXPECT_EQ(circuit1->Wqb[i]->toString(), circuit2->Wqb[i]->toString());
    EXPECT_EQ(circuit1->Wqc[i]->toString(), circuit2->Wqc[i]->toString());
    EXPECT_EQ(circuit1->Kq[i]->toString(), circuit2->Kq[i]->toString());
  }
  EXPECT_EQ(circuit1->A->toString(), circuit2->A->toString());
  EXPECT_EQ(circuit1->B->toString(), circuit2->B->toString());
  EXPECT_EQ(circuit1->C->toString(), circuit2->C->toString());

  circuit2->Wqa[0]->values[0][0] = Integer::TWO();
  EXPECT_EQ(circuit1->Wqa[0]->toString(), "[[\"1\",\"0\",\"0\",\"0\",\"0\",\"0\"]]");
  EXPECT_EQ(circuit2->Wqa[0]->toString(), "[[\"2\",\"0\",\"0\",\"0\",\"0\",\"0\"]]");

  circuit2->Kq[0] = make_shared<IntegerImpl>(100);
  EXPECT_EQ(circuit1->Kq[0]->toString(), "7");
  EXPECT_EQ(circuit2->Kq[0]->toString(), "100");

  circuit2->A->values[0][0] = Integer::TWO();
  EXPECT_EQ(circuit1->A->toString(), "[[\"1\",\"2\",\"0\",\"0\",\"0\",\"0\"]]");
  EXPECT_EQ(circuit2->A->toString(), "[[\"2\",\"2\",\"0\",\"0\",\"0\",\"0\"]]");
}

TEST(CBase, Shift)
{
  size_t Q = 2;
  vector<shared_ptr<Matrix>> Wqa;
  vector<shared_ptr<Matrix>> Wqb;
  vector<shared_ptr<Matrix>> Wqc;
  vector<shared_ptr<Integer>> Kq;

  Wqa.push_back(make_shared<Matrix>(vector<int>({1, 0, 0, 0, 0, 0})));
  Wqa.push_back(make_shared<Matrix>(vector<int>({0, 1, 0, 0, 0, 0})));
  Wqb.push_back(make_shared<Matrix>(vector<int>({0, 0, 1, 0, 0, 0})));
  Wqb.push_back(make_shared<Matrix>(vector<int>({0, 0, 0, 1, 0, 0})));
  Wqc.push_back(make_shared<Matrix>(vector<int>({0, 0, 0, 0, 1, 0})));
  Wqc.push_back(make_shared<Matrix>(vector<int>({0, 0, 0, 0, 0, 1})));
  Kq.push_back(make_shared<IntegerImpl>(7));
  Kq.push_back(make_shared<IntegerImpl>(9));

  auto A = make_shared<Matrix>(vector<int>({1, 2, 0, 0, 0, 0}));
  auto B = make_shared<Matrix>(vector<int>({0, 0, 3, 4, 0, 0}));
  auto C = make_shared<Matrix>(vector<int>({0, 0, 0, 0, 5, 6}));

  auto circuit = make_shared<CEnc>(GP_Q, GP_P, GP_G, Wqa, Wqb, Wqc, Kq, A, B, C);
  EXPECT_EQ(circuit->offset, 0);

  circuit->shift(2);

  EXPECT_EQ(circuit->GP_Q->toString(), GP_Q->toString());
  EXPECT_EQ(circuit->GP_P->toString(), GP_P->toString());
  EXPECT_EQ(circuit->GP_G->toString(), GP_G->toString());
  EXPECT_EQ(circuit->Wqa.size(), Wqa.size());
  EXPECT_EQ(circuit->Wqb.size(), Wqb.size());
  EXPECT_EQ(circuit->Wqc.size(), Wqc.size());
  EXPECT_EQ(circuit->Kq.size(), Kq.size());

  EXPECT_EQ(circuit->Wqa[0]->toString(), "[[\"0\",\"0\",\"1\",\"0\",\"0\",\"0\",\"0\",\"0\"]]");
  EXPECT_EQ(circuit->Wqa[1]->toString(), "[[\"0\",\"0\",\"0\",\"1\",\"0\",\"0\",\"0\",\"0\"]]");
  EXPECT_EQ(circuit->Wqb[0]->toString(), "[[\"0\",\"0\",\"0\",\"0\",\"1\",\"0\",\"0\",\"0\"]]");
  EXPECT_EQ(circuit->Wqb[1]->toString(), "[[\"0\",\"0\",\"0\",\"0\",\"0\",\"1\",\"0\",\"0\"]]");
  EXPECT_EQ(circuit->Wqc[0]->toString(), "[[\"0\",\"0\",\"0\",\"0\",\"0\",\"0\",\"1\",\"0\"]]");
  EXPECT_EQ(circuit->Wqc[1]->toString(), "[[\"0\",\"0\",\"0\",\"0\",\"0\",\"0\",\"0\",\"1\"]]");
  EXPECT_EQ(circuit->Kq[0]->toString(), Kq[0]->toString());
  EXPECT_EQ(circuit->Kq[1]->toString(), Kq[1]->toString());

  EXPECT_EQ(circuit->offset, 2);
  EXPECT_EQ(circuit->A->toString(), "[[\"1\",\"2\",\"0\",\"0\",\"0\",\"0\"]]");
  EXPECT_EQ(circuit->B->toString(), "[[\"0\",\"0\",\"3\",\"4\",\"0\",\"0\"]]");
  EXPECT_EQ(circuit->C->toString(), "[[\"0\",\"0\",\"0\",\"0\",\"5\",\"6\"]]");
}

} // namespace
