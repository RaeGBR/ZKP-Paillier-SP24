#pragma once

#include "../lib/namespace.hpp"

#include <stdexcept>
#include <vector>

#include "./CircuitZKPVerifier.hpp"
#include "./PolynomialCommitment.hpp"
#include "../lib/math/Matrix.hpp"
#include "../lib/math/Polynomial.hpp"

namespace polyu
{

class CircuitZKPProver
{
private:
  // [y^(M+1), y^(M+2), ... , y^(M+Q)]
  vector<shared_ptr<Integer>> cachedY_Mq;
  vector<shared_ptr<Integer>> getY_Mq(const shared_ptr<Integer> &y);
  shared_ptr<Integer> getY_Mq(const shared_ptr<Integer> &y, size_t q); // q: 1 to Q

  // [y^m, y^2m, ... , y^mn]
  shared_ptr<Matrix> cachedY_ = nullptr;
  shared_ptr<Matrix> getY_(const shared_ptr<Integer> &y);

  shared_ptr<Matrix> Wai(size_t i, const shared_ptr<Integer> &y);
  shared_ptr<Matrix> Wbi(size_t i, const shared_ptr<Integer> &y);
  shared_ptr<Matrix> Wci(size_t i, const shared_ptr<Integer> &y);
  shared_ptr<Integer> K(const shared_ptr<Integer> &y);

public:
  shared_ptr<CircuitZKPVerifier> zkp;

  shared_ptr<Matrix> A;
  shared_ptr<Matrix> B;
  shared_ptr<Matrix> C;
  vector<shared_ptr<Integer>> D;

  vector<shared_ptr<Integer>> randA;
  vector<shared_ptr<Integer>> randB;
  vector<shared_ptr<Integer>> randC;
  shared_ptr<Integer> randD;

  vector<shared_ptr<Integer>> Tx;
  vector<shared_ptr<Integer>> TxRi;

  CircuitZKPProver(
      const shared_ptr<CircuitZKPVerifier> &zkp,
      const shared_ptr<Matrix> &A,
      const shared_ptr<Matrix> &B,
      const shared_ptr<Matrix> &C);

  vector<shared_ptr<Integer>> commit();
  vector<shared_ptr<Integer>> polyCommit(const shared_ptr<Integer> &y);
};

} // namespace polyu
