#include "gtest/gtest.h"

#include "app/namespace.hpp"

#include <set>
#include "app/PaillierEncryption.hpp"
#include "app/utils/ConvertUtils.hpp"

namespace
{

TEST(Paillier, KeyGeneration)
{
  // check key length correct
  for (size_t i = 1; i < 32; i++)
  {

    auto crypto = make_shared<PaillierEncryption>(i);
    EXPECT_EQ(NumBytes(crypto->getPublicKey()), i);
    EXPECT_EQ(GCD(crypto->n, crypto->lambda), 1);
  }

  // check seedable key gen
  vector<uint8_t> seed{0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34};
  auto c1 = make_shared<PaillierEncryption>(32, seed);
  auto c2 = make_shared<PaillierEncryption>(32, seed);
  EXPECT_EQ(c1->p, c2->p);
  EXPECT_EQ(c1->q, c2->q);
  EXPECT_EQ(c1->n2, c2->n2);
  EXPECT_EQ(GCD(c1->n, c1->lambda), 1);
}

TEST(Paillier, CyclicGroupGeneration)
{
  // p = 3, q = 5, N = 15
  auto crypto = make_shared<PaillierEncryption>(ZZ(15), ZZ(3), ZZ(5));

  // GP_P = N^2 = 225
  // GP_Q = (f * GP_P) + 1 is prime
  auto Q = crypto->Q;
  auto p = crypto->n2;
  auto g = crypto->G;

  EXPECT_EQ(p, 225);
  EXPECT_EQ(Q, 1801);

  EXPECT_FALSE(ProbPrime(p));
  EXPECT_TRUE(ProbPrime(Q));

  ZZ_p::init(Q);
  EXPECT_EQ(power(g, p), 1);
  EXPECT_EQ(power(g, 0), 1);

  for (int i = 1; i != p; i++)
  {
    EXPECT_NE(power(g, i), 1)
        << "generator: " << g << "^" << i << " mod " << Q
        << " should not equal to 1";
  }
}

TEST(Paillier, PickRandom)
{
  size_t byteLength = 2;
  auto crypto = make_shared<PaillierEncryption>(byteLength);
  auto N = crypto->n;

  for (int i = 0; i < 1000; i++)
  {
    auto r = conv<ZZ>(crypto->pickRandom());
    EXPECT_TRUE(r > conv<ZZ>(0));
    EXPECT_TRUE(r < N);
    EXPECT_TRUE(GCD(r, N) == 1);
  }
}

TEST(Paillier, EncryptionDecryption)
{
  size_t byteLength = 32;
  ZZ m;
  m = 30;

  auto crypto = make_shared<PaillierEncryption>(byteLength);

  for (int i = 0; i < 50; i++)
  {
    auto r = crypto->pickRandom();
    auto c = crypto->encrypt(m, r); // r is optional
    EXPECT_EQ(crypto->decrypt(c), m);
  }
  for (int i = 0; i < 50; i++)
  {
    auto c = crypto->encrypt(m); // r is optional
    EXPECT_EQ(crypto->decrypt(c), m);
  }
}

TEST(Paillier, EncryptionDecryption2)
{
  size_t byteLength = 32;
  string msg = "testing";
  ZZ m = ConvertUtils::toZZ(ConvertUtils::toBinary(msg));

  auto crypto = make_shared<PaillierEncryption>(byteLength);
  auto pk = crypto->getPublicKey();
  auto sk1 = crypto->getPrivateElement1();
  auto sk2 = crypto->getPrivateElement2();
  auto GP_Q = crypto->getGroupQ();
  auto GP_P = crypto->getGroupP();
  auto GP_G = crypto->getGroupG();

  auto encryptor = make_shared<PaillierEncryption>(pk, GP_Q, GP_P, GP_G);
  auto decryptor = make_shared<PaillierEncryption>(pk, sk1, sk2, GP_Q, GP_P, GP_G);

  auto c = encryptor->encrypt(m);
  auto r = decryptor->decrypt(c);

  auto rBin = ConvertUtils::toBinary(r);
  auto ret = ConvertUtils::toString(rBin);

  EXPECT_EQ(encryptor->getPublicKey(), decryptor->getPublicKey());
  EXPECT_EQ(encryptor->getGroupQ(), decryptor->getGroupQ());
  EXPECT_EQ(encryptor->getGroupP(), decryptor->getGroupP());
  EXPECT_EQ(encryptor->getGroupG(), decryptor->getGroupG());
  EXPECT_EQ(m, r);
  EXPECT_EQ(ret, msg);
}

TEST(Paillier, EncryptionDecryption3)
{
  size_t byteLength = 32;
  string msg = "testing";
  ZZ m = ConvertUtils::toZZ(ConvertUtils::toBinary(msg));

  auto crypto = make_shared<PaillierEncryption>(byteLength);
  auto pk = crypto->getPublicKey();
  auto sk1 = crypto->getPrivateElement1();
  auto sk2 = crypto->getPrivateElement2();
  auto GP_Q = crypto->getGroupQ();
  auto GP_P = crypto->getGroupP();
  auto GP_G = crypto->getGroupG();

  auto encryptor = make_shared<PaillierEncryption>(pk, GP_Q, GP_P, GP_G);
  auto decryptor = crypto;

  auto c = encryptor->encrypt(m);
  auto r = decryptor->decrypt(c);

  auto rBin = ConvertUtils::toBinary(r);
  auto ret = ConvertUtils::toString(rBin);

  EXPECT_EQ(encryptor->getPublicKey(), decryptor->getPublicKey());
  EXPECT_EQ(encryptor->getGroupQ(), decryptor->getGroupQ());
  EXPECT_EQ(encryptor->getGroupP(), decryptor->getGroupP());
  EXPECT_EQ(encryptor->getGroupG(), decryptor->getGroupG());
  EXPECT_EQ(m, r);
  EXPECT_EQ(ret, msg);
}

TEST(Paillier, HomomorphicAddition)
{
  size_t byteLength = 32;
  auto m1 = conv<ZZ>("30");
  auto m2 = conv<ZZ>("32");

  auto crypto = make_shared<PaillierEncryption>(byteLength);

  EXPECT_EQ(crypto->decrypt(crypto->add(crypto->encrypt(m1), m2)), 62);
  EXPECT_EQ(crypto->decrypt(crypto->add(crypto->encrypt(m1), crypto->encrypt(m2))), 62);

  ZZ_p::init(crypto->n);
  for (int i = 0; i < 50; i++)
  {
    auto ma = RandomBits_ZZ(32);
    auto mb = RandomBits_ZZ(32);
    auto sum = ma + mb;
    EXPECT_EQ(crypto->decrypt(crypto->add(crypto->encrypt(ma), mb)), sum);
    EXPECT_EQ(crypto->decrypt(crypto->add(crypto->encrypt(ma), crypto->encrypt(mb))), sum);
  }
}

TEST(Paillier, HomomorphicMultiplication)
{
  size_t byteLength = 32;
  auto m1 = conv<ZZ>("30");
  auto m2 = conv<ZZ>("32");

  auto crypto = make_shared<PaillierEncryption>(byteLength);

  EXPECT_EQ(crypto->decrypt(crypto->mul(crypto->encrypt(m1), m2)), 960);

  ZZ_p::init(crypto->n);
  for (int i = 0; i < 50; i++)
  {
    auto ma = RandomBits_ZZ(32);
    auto mb = RandomBits_ZZ(32);
    auto prod = ma * mb;

    EXPECT_EQ(crypto->decrypt(crypto->mul(crypto->encrypt(ma), mb)), prod);
  }
}

TEST(Paillier, Generate_group_element)
{
  size_t byteLength = 2;
  auto crypto = make_shared<PaillierEncryption>(byteLength);

  auto Q = crypto->getGroupQ();
  auto p = crypto->getGroupP();
  auto g = crypto->getGroupG();
  auto pk = crypto->getPublicKey();
  auto sk1 = crypto->getPrivateElement1();
  auto sk2 = crypto->getPrivateElement2();

  printf("byteLength: %zu\n\n", byteLength);
  printf("p=N^2 (%lu-bit):\n%s\n\n", NumBits(p), ConvertUtils::toString(p).c_str());
  printf("Q (%lu-bit):\n%s\n\n", NumBits(Q), ConvertUtils::toString(Q).c_str());
  printf("G:\n%s\n\n", ConvertUtils::toString(g).c_str());
  printf("pk=N (%lu-bit):\n%s\n\n", NumBits(pk), ConvertUtils::toString(pk).c_str());
  printf("sk1=p (%lu-bit):\n%s\n\n", NumBits(sk1), ConvertUtils::toString(sk1).c_str());
  printf("sk2=q (%lu-bit):\n%s\n\n", NumBits(sk2), ConvertUtils::toString(sk2).c_str());
}

TEST(Paillier, Encrypt_decrypt_with_predefined_group)
{
  auto pk = ConvertUtils::toZZ("26073256271874186071125825914454935991483470802698567902720068833093596691811178134045207207596113005364237384029927538957512041216365361231778576719728684525680259101219662135354615006125020396411281192169709202061931869567014439274168275655072152029813100964481105821175191330034832669432404296770201059705302550720010746147453045171840510967537019382440326171418313990505494310422702412744957232673472028430228082518167001114220146490437986537032133279703709111411184899245220853112001028533066537741546490668094431783002965389853514131097481390405162750043722627342380400218852618564710439067402776609850056021277");
  auto sk1 = ConvertUtils::toZZ("160623637288235409827527352398419390921447002432910643309058541999597443714488505297919461146354728013261558840461091417221542788366961870160294968811844525216585313996963439374350229070974681037083868966485722188434747916524656403749063093539107116072286834400977630550169364258609977972582068794220117019439");
  auto sk2 = ConvertUtils::toZZ("162325151590773215258746784973623658599835223142457774666418202435132544662623364651803170775479615618958928336272524476540378719901966881413147892606811120046314293652879386688338345391570285017746183881331833328941218127586857292535706117123490492785590638950840132653143800554932430352269203309899292598643");
  auto GP_Q = ConvertUtils::toZZ("2246107744412603021521892070249685776301711920243212783693004417922119725101580299469967408695023925125981273266914754293214496598590440949025064195640492343219687270542490490232427124948311993246492161003102465417903916316134331480506755927146410930443978852789649485293438286268936159305473522713034897458658467236421890048290803731465078935673273378101858499730505280602765583660698345287930919199009433655382433366854278931737521488278163503700190078757842122401012740280717335540247927060028490968516744131942069319773862172928435856002980625726689919423117927267748537956913954905504581567708375840077681150373709830726908307107882441344909900376618511346977933888097398328511328935537806798220696442252495226515556283538900737465004666291985637289150290818459868187291586004545637719746402880253931295624055259653755849919128469564268160873658431927238193998047506555425454573759733787425625366687835038961302122144436127467958399719378073792228501883047393290802662090221596361008692541093791153711648033399671431681874708187933109419375314339598599600103544147383620211663510008718893800053627746940279297645686013605496453795929236248666389294307512803209428541190279944074432567137126932548336069926668887129929216607052248617");
  auto GP_P = ConvertUtils::toZZ("679814692618826580363768786395183346338290532761262949059626034480060449485950453834735898515443076611979804257540785197704145459621804161327198606428720442863101474135136346922647434911716704977751864710382102124062928667110875145431826854463199434153746626146988343006488585432486731024659056511209109400320359333057472774906417594269091687552443516374654509603663825848294668178177465280850762469433847958650857556553958514448402387493390890950420726016296041888926374176972559182883755163446879833086181637996994346178529713356064121066277429094034479244285086945444472747249986351544970208144181549660315118151849222374972247914008002828362560646676304887099858924968946225336358636663985108420307639906929547976863281942766567029359765826872166249742824097596812405354596248349163958761017820900100271072655950258400680968259222023083583799533423706791220943718978981666299810459967853337053682411572348353905000649042411461246489019182225724040103475498605717555285136265616332024422681929113545312242140859464719032044403204580238928382359061621852179208094475600369313457478816198212409217199681277324242628839592495610306838961633247175057292465954238259512270336041145300978379884118320989205832302260559058695283476710729");
  auto GP_G = ConvertUtils::toZZ_p("875060672959374630998373930072861394480471913042256194354952385224300853851305693047514427994473442021676331913820299148953070270861184782501989179518206183367840892686771951255741697908827295854529393095186993998635293823238498785306675805945977295323592223502591027264267395375163413988749183177078241977221856785926484958500134247393800287402380899554440250891814973868222537579711061743922042198638023169889435025008735653447686571897830231172057815323221499016958094746761024170312790387741085822670770237546331005110746946383256775587758535829772114000240218780041442484391134479828782745645920464273381044767399851218901688304440161289418192271703569484198943522175026104609823460808123890427783623999950303572140765674077675090704499962919555465934179337516876203237676270796438479914185838255717134156731022706607007409303515585576043673919285585604762375088870863152596059870687412793036895931991839304972668681934458021422497602376432956537744075284668006683020830151872816744632057170777086352010635803395416808333245848844924182063515142867947819056767176640778214900788666080638963351341717925100626687605159100330716538918293466365032152805547847940042109739688503095302074393016391956492007823236949029172220711947987033");

  string msg = "testing";
  auto msgBin = ConvertUtils::toBinary(msg);
  auto m = ConvertUtils::toZZ(msgBin);

  auto encryptor = make_shared<PaillierEncryption>(pk, GP_Q, GP_P, GP_G);
  auto decryptor = make_shared<PaillierEncryption>(pk, sk1, sk2, GP_Q, GP_P, GP_G);

  auto c = encryptor->encrypt(m);
  auto r = decryptor->decrypt(c);

  auto rBin = ConvertUtils::toBinary(r);
  auto ret = ConvertUtils::toString(rBin);

  EXPECT_EQ(m, r);
  EXPECT_EQ(ret, msg);
}

} // namespace
