#pragma once

#include "./namespace.hpp"

#include <NTL/ZZ.h>
#include <NTL/ZZ_p.h>
#include <NTL/vector.h>
#include <NTL/matrix.h>

#include "./utils/ConvertUtils.hpp"
#include "./CircuitZKPVerifier.hpp"
#include "./CircuitZKPProver.hpp"
#include "./math/Matrix.hpp"

namespace polyu
{
/**
 * @brief _CBase_ refers to the basic interface of a circuit. The paillier group elements (_GP_Q_, _GP_P_, _GP_G_) and linear constrains (_Wqa_, _Wqb_, _Wqc_ and _Kq_) are common inputs for both prover and verifier, while the circuit arguments' assignments (_A_, _B_, _C_) are the secret inputs for prover. The ZKP protocol run on a _CBase_ instance.
 */
class CBase
{
private:
  void convertWire(const vector<shared_ptr<Matrix>> &source,
                   map<size_t, map<size_t, map<size_t, ZZ_p>>> &target,
                   size_t m, size_t n);

public:
  /**
   * @brief Copy a circuit
   *
   * @param values Source values
   * @param target Target values
   */
  static void copyCircuit(const shared_ptr<CBase> &values, const shared_ptr<CBase> &target);

  /// @brief  Group element Q
  ZZ GP_Q;

  /// @brief  Group element p
  ZZ GP_P;

  /// @brief  Group generator g
  ZZ_p GP_G;

  /// @brief  Linear constrains w_q,a
  vector<shared_ptr<Matrix>> Wqa;

  /// @brief  Linear constrains w_q,b
  vector<shared_ptr<Matrix>> Wqb;

  /// @brief  Linear constrains w_q,c
  vector<shared_ptr<Matrix>> Wqc;

  /// @brief  Linear constrains K_q
  Vec<ZZ_p> Kq;

  /// @brief  Circuit arguments' value assignment A
  shared_ptr<Matrix> A = nullptr;

  /// @brief  Circuit arguments' value assignment B
  shared_ptr<Matrix> B = nullptr;

  /// @brief  Circuit arguments' value assignment C
  shared_ptr<Matrix> C = nullptr;

  /// @brief  Total number of linear constrains
  size_t linearCount = 0;

  /// @brief  Total number of multiplication constrains
  size_t gateCount = 0;

  /// @brief  Constrains offset, for append circuit
  size_t offset = 0;

  /**
   * @brief Construct a new CBase object
   */
  explicit CBase();

  /**
   * @brief Construct a new CBase object
   *
   * @param GP_Q Group element Q
   * @param GP_P Group element p
   * @param GP_G Group generator g
   */
  explicit CBase(
      const ZZ &GP_Q,
      const ZZ &GP_P,
      const ZZ_p &GP_G);

  /**
   * @brief Construct a new CBase object
   *
   * @param GP_Q Group element Q
   * @param GP_P Group element p
   * @param GP_G Group generator g
   * @param Wqa Linear constrains w_q,a
   * @param Wqb Linear constrains w_q,b
   * @param Wqc Linear constrains w_q,c
   * @param Kq Linear constrains K_q
   * @param A Circuit arguments' value assignment A
   * @param B Circuit arguments' value assignment B
   * @param C Circuit arguments' value assignment C
   */
  explicit CBase(
      const ZZ &GP_Q,
      const ZZ &GP_P,
      const ZZ_p &GP_G,
      const vector<shared_ptr<Matrix>> &Wqa,
      const vector<shared_ptr<Matrix>> &Wqb,
      const vector<shared_ptr<Matrix>> &Wqc,
      const Vec<ZZ_p> &Kq,
      const shared_ptr<Matrix> &A = nullptr,
      const shared_ptr<Matrix> &B = nullptr,
      const shared_ptr<Matrix> &C = nullptr);

  /// @private
  void shift(size_t n);

  /// @private
  void group(size_t n, size_t m = 0);

  /// @private
  void trim();

  /// @private
  void append(const shared_ptr<CBase> &b);

  /// @private
  size_t assignValues(const shared_ptr<CBase> &b, size_t offset = 0);

  /// @private
  size_t addGate(size_t n = 1);

  /// @private
  size_t addLinear();

  /**
   * @brief Generate CircuitZKPVerifier object
   *
   * @param gi Generators used in PolynomialCommitment scheme
   * @return shared_ptr<CircuitZKPVerifier>
   */
  shared_ptr<CircuitZKPVerifier> generateVerifier(const Vec<ZZ_p> &gi);

  /**
   * @brief Generate CircuitZKPProver object
   *
   * @param gi Generators used in PolynomialCommitment scheme
   * @return shared_ptr<CircuitZKPProver>
   */
  shared_ptr<CircuitZKPProver> generateProver(const Vec<ZZ_p> &gi);

  /// @private
  json toJson();

  string toString();
};

} // namespace polyu
