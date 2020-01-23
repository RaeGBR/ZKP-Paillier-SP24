#pragma once

#include "../lib/namespace.hpp"

#include <stdexcept>
#include <vector>

#include "./CircuitZKPVerifier.hpp"
#include "./PolynomialCommitment.hpp"

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

  CircuitZKPProver(
      const shared_ptr<CircuitZKPVerifier> &zkp,
      const shared_ptr<Matrix> &A,
      const shared_ptr<Matrix> &B,
      const shared_ptr<Matrix> &C);

  vector<shared_ptr<Integer>> commit();
};

} // namespace polyu
