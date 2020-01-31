#pragma once

#include "../lib/namespace.hpp"

#include <stdexcept>
#include <vector>
#include <math.h>

#include "./PolynomialCommitment.hpp"
#include "../lib/math/Random.hpp"
#include "../lib/math/IntegerImpl.hpp"
#include "../lib/math/Matrix.hpp"
#include "../lib/math/Polynomial.hpp"
#include "../lib/utils/HexUtils.hpp"

namespace polyu
{

class CircuitZKPVerifier
{
private:
  // [1, y, y^2, ... , y^m]
  shared_ptr<Matrix> cachedY = nullptr;

  // [y^m, y^2m, ... , y^mn]
  shared_ptr<Matrix> cachedY_ = nullptr;

  // [y^(M+1), y^(M+2), ... , y^(M+Q)]
  vector<shared_ptr<Integer>> cachedY_Mq;

  vector<size_t> calcM1M2N(size_t m);
  vector<shared_ptr<Integer>> getY_Mq(const shared_ptr<Integer> &y);
  shared_ptr<Integer> getY_Mq(const shared_ptr<Integer> &y, size_t q); // q: 1 to Q

public:
  static vector<size_t> calcMN(size_t N);

  shared_ptr<Integer> GP_Q;
  shared_ptr<Integer> GP_P;
  shared_ptr<Integer> GP_G;

  vector<shared_ptr<Matrix>> Wqa;
  vector<shared_ptr<Matrix>> Wqb;
  vector<shared_ptr<Matrix>> Wqc;
  vector<shared_ptr<Integer>> Kq;

  size_t N;
  size_t m;
  size_t n;
  size_t M;
  size_t Q;

  vector<shared_ptr<Integer>> commitA;
  vector<shared_ptr<Integer>> commitB;
  vector<shared_ptr<Integer>> commitC;
  shared_ptr<Integer> commitD;

  shared_ptr<PolynomialCommitment> commitScheme;

  size_t txM1;
  size_t txM2;
  size_t txN;

  vector<shared_ptr<Integer>> pc;

  CircuitZKPVerifier(
      const shared_ptr<Integer> &GP_Q,
      const shared_ptr<Integer> &GP_P,
      const shared_ptr<Integer> &GP_G,
      const vector<shared_ptr<Matrix>> &Wqa,
      const vector<shared_ptr<Matrix>> &Wqb,
      const vector<shared_ptr<Matrix>> &Wqc,
      const vector<shared_ptr<Integer>> &Kq);

  shared_ptr<Matrix> getY(const shared_ptr<Integer> &y);
  shared_ptr<Matrix> getY_(const shared_ptr<Integer> &y);
  void setY(const shared_ptr<Integer> &y); // recalculated cachedY, cachedY_, cachedY_Mq

  shared_ptr<Matrix> Wai(size_t i, const shared_ptr<Integer> &y);
  shared_ptr<Matrix> Wbi(size_t i, const shared_ptr<Integer> &y);
  shared_ptr<Matrix> Wci(size_t i, const shared_ptr<Integer> &y);
  shared_ptr<Integer> K(const shared_ptr<Integer> &y);
  shared_ptr<Polynomial> createSx(const shared_ptr<Integer> &y);

  void setCommits(const vector<shared_ptr<Integer>> &commits);
  shared_ptr<Integer> calculateY();

  void setPolyCommits(const vector<shared_ptr<Integer>> &pc);
  shared_ptr<Integer> calculateX();

  bool verify(const vector<shared_ptr<Integer>> &proofs, const shared_ptr<Integer> &y, const shared_ptr<Integer> &x);
};

} // namespace polyu
