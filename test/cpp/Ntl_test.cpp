#include "gtest/gtest.h"

#include <NTL/ZZ.h>
#include <NTL/ZZ_p.h>
#include <NTL/ZZX.h>

#include "lib/namespace.hpp"

#include "lib/math/IntegerImpl.hpp"
#include "lib/math/Matrix.hpp"
#include "lib/math/Polynomial.hpp"
#include "lib/math/Random.hpp"
#include "lib/utils/Timer.hpp"
#include "lib/paillier/PaillierEncryption.hpp"

using namespace cryptoplus;
using namespace NTL;

namespace
{

TEST(Ntl, test)
{
  ZZ c = conv<ZZ>("7");
  ZZ_p::init(c);

  ZZ_p a = conv<ZZ_p>("2");
  ZZ_p b = conv<ZZ_p>("5");

  auto ret = power(a, conv<ZZ>(b));
  cout << "ret: " << ret << endl;

  // ZZ_pX f1;
  // ZZ_pX f2;

  // SetCoeff(f1, 0, 5);
  // SetCoeff(f1, 2, a);
  // SetCoeff(f2, 0, 3);
  // SetCoeff(f2, 2, b);

  // cout << "f1: " << f1 << endl;
  // cout << "f2: " << f2 << endl;

  // auto ret = f1 * f2;

  // cout << "ret: " << ret << endl;

  // ZZ_p ret = a + b;
  // ZZ_p expect = conv<ZZ_p>("4");

  // cout << "a: " << a << endl;
  // cout << "b: " << b << endl;
  // cout << "c: " << c << endl;
  // cout << "ret: " << ret << endl;

  // EXPECT_EQ(ret, expect);
}

TEST(Ntl, mul)
{
  size_t m = 1;
  size_t n = 2;
  auto p = make_shared<IntegerImpl>(101);

  auto ky = make_shared<IntegerImpl>(-39)->mod(p);
  auto kyMatrix = make_shared<Matrix>(vector<shared_ptr<Integer>>({ky}));
  auto kyPoly = make_shared<Polynomial>();
  kyPoly->put(0, kyMatrix);

  auto rx = make_shared<Polynomial>();
  rx->put(-1, make_shared<Matrix>(vector<int>({1, 2})));
  rx->put(1, make_shared<Matrix>(vector<int>({3, 4})));

  auto rx_ = make_shared<Polynomial>();
  rx_->put(-1, make_shared<Matrix>(vector<int>({5, 6})));
  rx_->put(2, make_shared<Matrix>(vector<int>({7, 8})));

  auto tx = rx->mul(rx_, p);
  auto tx0 = tx->get(0);

  cout << "rx: " << rx->toString() << endl;
  cout << "rx_: " << rx_->toString() << endl;
  cout << "tx: " << tx->toString() << endl;
  cout << "ky: " << ky->toString() << endl;

  tx = tx->add(kyPoly, p);
  cout << "tx + ky: " << tx->toString() << endl;

  ZZ zzP = conv<ZZ>(p->toString().c_str());
  ZZ_p::init(zzP); // init mod p
  vector<ZZ_pX> rXs;
  vector<ZZ_pX> rX_s;

  for (size_t i = 0; i < n; i++)
  {
    rXs.push_back(ZZ_pX());
    rX_s.push_back(ZZ_pX());
  }

  int rxD0 = rx->getSmallestDegree();
  int rxDn = rx->getLargestDegree();
  int rx_D0 = rx_->getSmallestDegree();
  int rx_Dn = rx_->getLargestDegree();
  cout << "rxD0: " << rxD0 << endl;
  cout << "rx_D0: " << rx_D0 << endl;
  for (auto d : rx->getDegrees())
  {
    auto vec = rx->get(d)->values[0];
    int zzD = d - rxD0;
    cout << "d: " << d << ", zzD: " << zzD << endl;
    for (auto it : vec)
    {
      size_t j = it.first;
      auto v = it.second;
      ZZ_p zz = conv<ZZ_p>(v->toString().c_str());
      SetCoeff(rXs[j], zzD, zz);
    }
  }
  for (auto d : rx_->getDegrees())
  {
    auto vec = rx_->get(d)->values[0];
    int zzD = d - rx_D0;
    cout << "d: " << d << ", zzD: " << zzD << endl;
    for (auto it : vec)
    {
      size_t j = it.first;
      auto v = it.second;
      ZZ_p zz = conv<ZZ_p>(v->toString().c_str());
      SetCoeff(rX_s[j], zzD, zz);
    }
  }

  for (size_t j = 0; j < n; j++)
  {
    cout << "j: " << j << "; ";
    for (size_t i = 0; i <= rxDn - rxD0; i++)
    {
      int d = i + rxD0;
      cout << "i: " << i << ", f(" << d << "): " << rXs[j][i] << "; ";
    }
    cout << endl;
  }

  for (size_t j = 0; j < n; j++)
  {
    cout << "j: " << j << "; ";
    for (size_t i = 0; i <= rx_Dn - rx_D0; i++)
    {
      int d = i + rx_D0;
      cout << "i: " << i << ", f(" << d << "): " << rX_s[j][i] << "; ";
    }
    cout << endl;
  }

  ZZ_pX rrX;
  ZZ_pX f;
  for (size_t i = 0; i < n; i++)
  {
    mul(f, rXs[i], rX_s[i]);
    add(rrX, rrX, f);
  }

  stringstream ss;
  ss.str("");
  ss << zzP;
  cout << "zzP: " << zzP << endl;
  cout << "p: " << p->toString() << endl;

  auto maxD = rxDn + rx_Dn - rxD0 - rx_D0;
  for (size_t i = 0; i <= maxD; i++)
  {
    int d = i + rxD0 + rx_D0;

    cout << "i: " << i << ", rrX(" << d << "): " << rrX[i] << "; ";
  }
  cout << endl;
}

/*/
TEST(Ntl, Speed_test)
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

      ZZ zzQ = conv<ZZ>(GP_Q->toString().c_str());
      ZZ_p::init(zzQ);
      ZZ_p zzG = conv<ZZ_p>(GP_G->toString().c_str());
      ZZ_p zzA = conv<ZZ_p>(a->toString().c_str());
      ZZ_p zzB = conv<ZZ_p>(b->toString().c_str());

      size_t runTest = runTestMax;
      Timer::start("mod");
      for (size_t i = 0; i < runTestMax; i++)
      {
        zzA = zzA * zzB;
      }
      diff = Timer::end("mod", true);
      throughput = runTest / diff;
      mulThroughput += throughput;
      EXPECT_TRUE(a->gt(Integer::ZERO()));

      zzB = zzA;
      zzA = zzG;
      runTest = runTestMax / byteLength;
      Timer::start("pow");
      ZZ zzb = conv<ZZ>(zzB);
      for (size_t i = 0; i < runTest; i++)
      {
        zzA = power(zzA, zzb);
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
