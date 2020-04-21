#pragma once

#include "./namespace.hpp"

#include <NTL/ZZ.h>
#include <NTL/ZZ_p.h>
#include <NTL/ZZ_pX.h>
#include <NTL/vector.h>
#include <NTL/matrix.h>

#include "./CircuitZKPVerifier.hpp"
#include "./PolynomialCommitment.hpp"
#include "./math/MathUtils.hpp"
#include "./utils/ConvertUtils.hpp"
#include "./math/Matrix.hpp"
#include "./utils/Timer.hpp"

namespace polyu
{

class CircuitZKPProver
{
private:
public:
  /// @brief Common ZKP functions
  shared_ptr<CircuitZKPVerifier> zkp;

  /// @brief Circuit arguments' value assignment A
  Mat<ZZ_p> A;

  /// @brief Circuit arguments' value assignment B
  Mat<ZZ_p> B;

  /// @brief Circuit arguments' value assignment C
  Mat<ZZ_p> C;

  /// @brief Randomness vector D
  Vec<ZZ_p> D;

  /// @brief Randomness for commit(A)
  Vec<ZZ_p> randA;

  /// @brief Randomness for commit(B)
  Vec<ZZ_p> randB;

  /// @brief Randomness for commit(C)
  Vec<ZZ_p> randC;

  /// @brief Randomness for commit(D)
  ZZ_p randD;

  /// @brief Polynomial t(x)'s coefficient matrix T
  Mat<ZZ_p> txT;

  /// @brief Randomness for matrix T
  Vec<ZZ_p> txRi;

  /**
   * @brief Construct a new Circuit ZKP Prover object
   *
   * @param zkp Common ZKP function
   * @param A Circuit arguments' value assignment A
   * @param B Circuit arguments' value assignment B
   * @param C Circuit arguments' value assignment C
   */
  CircuitZKPProver(
      const shared_ptr<CircuitZKPVerifier> &zkp,
      const shared_ptr<Matrix> &A,
      const shared_ptr<Matrix> &B,
      const shared_ptr<Matrix> &C);

  /**
   * @brief Construct a new Circuit ZKP Prover object
   *
   * @param zkp Common ZKP function
   * @param A Circuit arguments' value assignment A
   * @param B Circuit arguments' value assignment B
   * @param C Circuit arguments' value assignment C
   */
  CircuitZKPProver(
      const shared_ptr<CircuitZKPVerifier> &zkp,
      const Mat<ZZ_p> &A,
      const Mat<ZZ_p> &B,
      const Mat<ZZ_p> &C);

  /**
   * @brief Commit matrix A, B, C
   *
   * @param result
   */
  void commit(Vec<ZZ_p> &result);

  /**
   * @brief Polynomial commitments for t(x)
   *
   * @param y Challenge value (y)
   * @param result Polynomial commitments result (pc)
   */
  void polyCommit(const ZZ_p &y, Vec<ZZ_p> &result);

  /**
   * @brief Prove circuit
   *
   * @param y Challenge value (y)
   * @param x Challenge value (x)
   * @param result Proofs
   */
  void prove(const ZZ_p &y, const ZZ_p &x, Vec<ZZ_p> &result);
};

} // namespace polyu
