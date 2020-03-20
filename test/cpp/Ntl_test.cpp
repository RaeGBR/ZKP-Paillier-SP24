#include "gtest/gtest.h"

#include <NTL/ZZ.h>
#include <NTL/ZZ_p.h>
#include <NTL/ZZX.h>
#include <NTL/vector.h>
#include <NTL/matrix.h>

#include "lib/namespace.hpp"

#include "lib/math/IntegerImpl.hpp"
#include "lib/math/Matrix.hpp"
#include "lib/math/Polynomial.hpp"
#include "lib/math/Random.hpp"
#include "lib/utils/Timer.hpp"
#include "lib/paillier/PaillierEncryption.hpp"

#include "app/ConvertUtils.hpp"
#include "app/MathUtils.hpp"

using namespace cryptoplus;
using namespace NTL;

namespace
{

TEST(Ntl, test)
{
  ZZ p = ConvertUtils::toZZ(make_shared<IntegerImpl>(7));
  ZZ_p::init(p);

  ZZ_p a = ConvertUtils::toZZ_p(make_shared<IntegerImpl>(5));
  ZZ_p b = ConvertUtils::toZZ_p(make_shared<IntegerImpl>(6));
  ZZ_p c = a + b;

  vector<shared_ptr<Integer>> vecA;
  vecA.push_back(make_shared<IntegerImpl>(10));
  vecA.push_back(make_shared<IntegerImpl>(20));
  vecA.push_back(make_shared<IntegerImpl>(30));

  Vec<ZZ> vecAZZ;
  Vec<ZZ_p> vecAZZ_p;
  ConvertUtils::toVecZZ(vecA, vecAZZ);
  ConvertUtils::toVecZZ_p(vecA, vecAZZ_p);

  // Mat<ZZ> mat;
  // mat.SetDims(2, 3);

  // cout << mat << endl;

  // mat[0][1] = conv<ZZ>("2");
  // mat[1][2] = conv<ZZ>("3");
  // cout << mat << endl;

  // cout << mat.NumRows() << endl;
  // cout << mat.NumCols() << endl;

  // // ZZ_pX f;
  // // SetCoeff(f, 0, a);
  // // SetCoeff(f, 1, b);
  // // SetCoeff(f, 7, c);
  // // cout << f << endl;
  // // cout << deg(f) << endl;

  // // random(a);
  // // cout << "a: " << a << endl;
}

/*/
TEST(Ntl, Speed_test)
{

  const size_t groupTry = 5;
  const size_t runTestMax = 5000000;
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
      runTest = runTestMax / byteLength / 100;
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
