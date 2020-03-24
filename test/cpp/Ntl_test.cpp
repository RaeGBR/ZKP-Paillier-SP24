#include "gtest/gtest.h"

#include <NTL/ZZ.h>
#include <NTL/ZZ_p.h>
#include <NTL/ZZX.h>
#include <NTL/vector.h>
#include <NTL/matrix.h>

#include "lib/namespace.hpp"

#include "lib/math/Matrix.hpp"
#include "lib/utils/Timer.hpp"

#include "app/ConvertUtils.hpp"
#include "app/MathUtils.hpp"

using namespace cryptoplus;
using namespace NTL;

namespace
{

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
