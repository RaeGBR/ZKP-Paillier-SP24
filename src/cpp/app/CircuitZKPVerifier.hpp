#pragma once

#include "../lib/namespace.hpp"

#include <stdexcept>
#include <vector>
#include <math.h>

#include <NTL/ZZ.h>
#include <NTL/ZZ_p.h>
#include <NTL/ZZ_pX.h>
#include <NTL/vector.h>
#include <NTL/matrix.h>

#include "./PolynomialCommitment.hpp"
#include "./MathUtils.hpp"
#include "./ConvertUtils.hpp"
#include "../lib/math/Matrix.hpp"
#include "../lib/utils/Timer.hpp"

namespace polyu
{

class CircuitZKPVerifier
{
private:
  // [1, y, y^2, ... , y^m]
  Vec<ZZ_p> cachedY;

  // [y^m, y^2m, ... , y^mn]
  Vec<ZZ_p> cachedY_;

  // [y^(M+1), y^(M+2), ... , y^(M+Q)]
  Vec<ZZ_p> cachedY_Mq;

  Vec<ZZ_p> &getY_Mq(const ZZ_p &y);
  ZZ_p getY_Mq(const ZZ_p &y, size_t q); // q: 1 to Q

  void convertWire(const vector<shared_ptr<Matrix>> &source, map<size_t, map<size_t, map<size_t, ZZ_p>>> &target);

public:
  static vector<size_t> calcMN(size_t N);
  static vector<size_t> calcM1M2N(size_t m);

  ZZ GP_Q;
  ZZ GP_P;
  ZZ_p GP_G;

  map<size_t, map<size_t, map<size_t, ZZ_p>>> Wqa; // map(i, q, j)
  map<size_t, map<size_t, map<size_t, ZZ_p>>> Wqb; // map(i, q, j)
  map<size_t, map<size_t, map<size_t, ZZ_p>>> Wqc; // map(i, q, j)
  Vec<ZZ_p> Kq;

  size_t N;
  size_t m;
  size_t n;
  size_t M;
  size_t Q;

  Vec<ZZ_p> commitA;
  Vec<ZZ_p> commitB;
  Vec<ZZ_p> commitC;
  ZZ_p commitD;

  shared_ptr<PolynomialCommitment> commitScheme;

  size_t txM1;
  size_t txM2;
  size_t txN;

  Vec<ZZ_p> pc;

  CircuitZKPVerifier(
      const ZZ &GP_Q,
      const ZZ &GP_P,
      const ZZ_p &GP_G,
      const vector<shared_ptr<Matrix>> &Wqa,
      const vector<shared_ptr<Matrix>> &Wqb,
      const vector<shared_ptr<Matrix>> &Wqc,
      const Vec<ZZ_p> &Kq);

  CircuitZKPVerifier(
      const ZZ &GP_Q,
      const ZZ &GP_P,
      const ZZ_p &GP_G,
      const vector<shared_ptr<Matrix>> &Wqa,
      const vector<shared_ptr<Matrix>> &Wqb,
      const vector<shared_ptr<Matrix>> &Wqc,
      const Vec<ZZ_p> &Kq,
      size_t m,
      size_t n,
      size_t Q);

  Vec<ZZ_p> &getY(const ZZ_p &y);
  Vec<ZZ_p> &getY_(const ZZ_p &y);
  void setY(const ZZ_p &y); // recalculated cachedY, cachedY_, cachedY_Mq

  shared_ptr<Matrix> Wai(size_t i, const ZZ_p &y);
  shared_ptr<Matrix> Wbi(size_t i, const ZZ_p &y);
  shared_ptr<Matrix> Wci(size_t i, const ZZ_p &y);
  ZZ_p K(const ZZ_p &y);
  void createSx(const ZZ_p &y, vector<ZZ_pX> &output);

  void setCommits(const Vec<ZZ_p> &commits);
  ZZ_p calculateY();

  void setPolyCommits(const Vec<ZZ_p> &pc);
  ZZ_p calculateX();

  bool verify(const Vec<ZZ_p> &proofs, const ZZ_p &y, const ZZ_p &x);
};

} // namespace polyu
