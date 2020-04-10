#pragma once

#include "../lib/namespace.hpp"

#include <stdexcept>
#include <vector>

#include <NTL/ZZ.h>
#include <NTL/ZZ_p.h>
#include <NTL/ZZ_pX.h>
#include <NTL/vector.h>
#include <NTL/matrix.h>

#include "./CircuitZKPVerifier.hpp"
#include "./PolynomialCommitment.hpp"
#include "./MathUtils.hpp"
#include "./ConvertUtils.hpp"
#include "../lib/math/Matrix.hpp"
#include "../lib/utils/Timer.hpp"

using namespace std;
using namespace NTL;

namespace polyu
{

class CircuitZKPProver
{
private:
public:
  shared_ptr<CircuitZKPVerifier> zkp;

  Mat<ZZ_p> A;
  Mat<ZZ_p> B;
  Mat<ZZ_p> C;
  Vec<ZZ_p> D;

  Vec<ZZ_p> randA;
  Vec<ZZ_p> randB;
  Vec<ZZ_p> randC;
  ZZ_p randD;

  Mat<ZZ_p> txT;
  Vec<ZZ_p> txRi;

  CircuitZKPProver(
      const shared_ptr<CircuitZKPVerifier> &zkp,
      const shared_ptr<Matrix> &A,
      const shared_ptr<Matrix> &B,
      const shared_ptr<Matrix> &C);

  CircuitZKPProver(
      const shared_ptr<CircuitZKPVerifier> &zkp,
      const Mat<ZZ_p> &A,
      const Mat<ZZ_p> &B,
      const Mat<ZZ_p> &C);

  void commit(Vec<ZZ_p> &result);
  void polyCommit(const ZZ_p &y, Vec<ZZ_p> &result);
  void prove(const ZZ_p &y, const ZZ_p &x, Vec<ZZ_p> &result);
};

} // namespace polyu
