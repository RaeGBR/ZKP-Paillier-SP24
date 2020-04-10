#pragma once

#include "../lib/namespace.hpp"

#include <stdexcept>

#include <NTL/vector.h>
#include <NTL/matrix.h>
#include <NTL/ZZ.h>
#include <NTL/ZZ_p.h>
#include <NTL/ZZ_pX.h>

#include "./PaillierEncryption.hpp"
#include "./ConvertUtils.hpp"
#include "./MathUtils.hpp"

using namespace std;
using namespace NTL;

namespace polyu
{

class PolynomialCommitment
{
public:
  ZZ Q;         // Group order
  ZZ p;         // sub-group order, (Q-1) % p = 0
  ZZ_p g;       // base generator, (g^p) % Q = 1
  Vec<ZZ_p> gi; // [g0, g1, ... , gn]

  PolynomialCommitment(const shared_ptr<PaillierEncryption> &crypto, size_t n);
  PolynomialCommitment(const shared_ptr<PaillierEncryption> &crypto, const Vec<ZZ_p> &gi);
  PolynomialCommitment(const ZZ &Q, const ZZ &p, const ZZ_p &g, const Vec<ZZ_p> &gi);

  // FIXME: DEV_ONLY: DEPRECATED: in secure generators gi
  PolynomialCommitment(const ZZ &Q, const ZZ &p, const ZZ_p &g, size_t n);

  ZZ_p commit(const Vec<ZZ_p> &mi, const ZZ_p &r);
  void commit(const Mat<ZZ_p> &ms, const Vec<ZZ_p> &rs, Vec<ZZ_p> &ret);

  void calcT(
      size_t m1, size_t m2, size_t n,
      const ZZ_pX &tx, Mat<ZZ_p> &ret);

  void commit(
      size_t m1, size_t m2, size_t n,
      const Mat<ZZ_p> &T, Vec<ZZ_p> &ri, Vec<ZZ_p> &ret);

  void eval(
      size_t m1, size_t m2, size_t n,
      const Mat<ZZ_p> &T, const Vec<ZZ_p> &ri, const ZZ_p &x,
      Vec<ZZ_p> &result);

  bool verify(
      size_t m1, size_t m2, size_t n,
      const Vec<ZZ_p> &pc, const Vec<ZZ_p> &pe, const ZZ_p &x);

  ZZ_p calcV(size_t n, const Vec<ZZ_p> &pe, const ZZ_p &x);
};

} // namespace polyu

using namespace polyu;
