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

  shared_ptr<Matrix> txT;
  vector<shared_ptr<Integer>> txRi;

  CircuitZKPProver(
      const shared_ptr<CircuitZKPVerifier> &zkp,
      const shared_ptr<Matrix> &A,
      const shared_ptr<Matrix> &B,
      const shared_ptr<Matrix> &C);

  vector<shared_ptr<Integer>> commit();
  vector<shared_ptr<Integer>> polyCommit(const shared_ptr<Integer> &y);
  vector<shared_ptr<Integer>> prove(const shared_ptr<Integer> &y, const shared_ptr<Integer> &x);
};

} // namespace polyu
