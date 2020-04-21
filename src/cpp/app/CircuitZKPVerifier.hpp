#pragma once

#include "./namespace.hpp"

#include <math.h>

#include <NTL/ZZ.h>
#include <NTL/ZZ_p.h>
#include <NTL/ZZ_pX.h>
#include <NTL/vector.h>
#include <NTL/matrix.h>

#include "./PolynomialCommitment.hpp"
#include "./math/MathUtils.hpp"
#include "./utils/ConvertUtils.hpp"
#include "./math/Matrix.hpp"
#include "./utils/Timer.hpp"

namespace polyu
{

/**
 * @brief _CircuitZKPVerifier_ handles the ZKP for verifier, it contains functions needed by verifier and some common functions between prover and verifier. The ZKP will run on a circuit (_CBase_), and depends on a paillier group (_PaillierEncryption_) and a commitment scheme (_PolynomialCommitment_).
 */
class CircuitZKPVerifier
{
private:
  // Y = [1, y, y^2, ... , y^m]
  Vec<ZZ_p> cachedY;

  // Y' = [y^m, y^2m, ... , y^mn]
  Vec<ZZ_p> cachedY_;

  // Y_Mq = [y^(M+1), y^(M+2), ... , y^(M+Q)]
  Vec<ZZ_p> cachedY_Mq;

  Vec<ZZ_p> &getY_Mq(const ZZ_p &y);
  ZZ_p getY_Mq(const ZZ_p &y, size_t q); // q: 1 to Q

  void convertWire(const vector<shared_ptr<Matrix>> &source, map<size_t, map<size_t, map<size_t, ZZ_p>>> &target);

public:
  /**
   * @brief Calculate the matrix size (m * n) base on the number of multiplication gates (gateCount) in circuit
   *
   * @param N The number of multiplication gates (gateCount)
   * @return vector<size_t> [m, n]
   */
  static vector<size_t> calcMN(size_t N);

  /**
   * @brief Calculate the matrix size of T used in polynomial commitment
   *
   * @param m The matrix size (m)
   * @return vector<size_t> [m1, m2, n]
   */
  static vector<size_t> calcM1M2N(size_t m);

  /// @brief Group element Q
  ZZ GP_Q;

  /// @brief Group element p
  ZZ GP_P;

  /// @brief Group generator g
  ZZ_p GP_G;

  /// @brief Linear constrains w_q,a; stored as map(i, q, j)
  map<size_t, map<size_t, map<size_t, ZZ_p>>> Wqa;

  /// @brief Linear constrains w_q,b; stored as
  map<size_t, map<size_t, map<size_t, ZZ_p>>> Wqb;

  /// @brief Linear constrains w_q,c; stored as
  map<size_t, map<size_t, map<size_t, ZZ_p>>> Wqc;

  /// @brief Linear constrains K_q
  Vec<ZZ_p> Kq;

  /// @brief Circuit's multiplication constrains count
  size_t N;

  /// @brief Matrix size m
  size_t m;

  /// @brief Matrix size n
  size_t n;

  /// @brief M = N + m
  size_t M;

  /// @brief Circuit's linear constrains count
  size_t Q;

  /// @brief Commitment of circuit arsigment matrix A
  Vec<ZZ_p> commitA;

  /// @brief Commitment of circuit arsigment matrix B
  Vec<ZZ_p> commitB;

  /// @brief Commitment of circuit arsigment matrix C
  Vec<ZZ_p> commitC;

  /// @brief Commitment of randomness D
  ZZ_p commitD;

  /// @brief Polynomial commitment scheme
  shared_ptr<PolynomialCommitment> commitScheme;

  /// @brief Polynomial commitment settings (m1)
  size_t txM1;

  /// @brief Polynomial commitment settings (m2)
  size_t txM2;

  /// @brief Polynomial commitment settings (n)
  size_t txN;

  /// @brief Polynomial commitment (pc)
  Vec<ZZ_p> pc;

  /**
   * @brief Construct a new Circuit ZKP Verifier object
   *
   * @param GP_Q Group element Q
   * @param GP_P Group element p
   * @param GP_G Group generator g
   * @param Wqa Linear constrains w_q,a
   * @param Wqb Linear constrains w_q,b
   * @param Wqc Linear constrains w_q,c
   * @param Kq Linear constrains K_q
   */
  CircuitZKPVerifier(
      const ZZ &GP_Q,
      const ZZ &GP_P,
      const ZZ_p &GP_G,
      const vector<shared_ptr<Matrix>> &Wqa,
      const vector<shared_ptr<Matrix>> &Wqb,
      const vector<shared_ptr<Matrix>> &Wqc,
      const Vec<ZZ_p> &Kq);

  /**
   * @brief Construct a new Circuit ZKP Verifier object
   *
   * @param GP_Q Group element Q
   * @param GP_P Group element p
   * @param GP_G Group generator g
   * @param Wqa Linear constrains w_q,a
   * @param Wqb Linear constrains w_q,b
   * @param Wqc Linear constrains w_q,c
   * @param Kq Linear constrains K_q
   * @param m Matrix size m
   * @param n Matrix size n
   * @param Q Linear constrains count (Q)
   */
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

  /// @private
  Vec<ZZ_p> &getY(const ZZ_p &y);

  /// @private
  Vec<ZZ_p> &getY_(const ZZ_p &y);

  /// @private
  void setY(const ZZ_p &y); // recalculated cachedY, cachedY_, cachedY_Mq

  /**
   * @brief Function w_a,i(Y)
   *
   * @param i
   * @param y Challenge value (y)
   * @return shared_ptr<Matrix>
   */
  shared_ptr<Matrix> Wai(size_t i, const ZZ_p &y);

  /**
   * @brief Function w_b,i(Y)
   *
   * @param i
   * @param y Challenge value (y)
   * @return shared_ptr<Matrix>
   */
  shared_ptr<Matrix> Wbi(size_t i, const ZZ_p &y);

  /**
   * @brief Function w_c,i(Y)
   *
   * @param i
   * @param y Challenge value (y)
   * @return shared_ptr<Matrix>
   */
  shared_ptr<Matrix> Wci(size_t i, const ZZ_p &y);

  /**
   * @brief Function K(Y)
   *
   * @param y Challenge value (y)
   * @return ZZ_p
   */
  ZZ_p K(const ZZ_p &y);

  /**
   * @brief Create polynomial s(X)
   *
   * @param y Challenge value (y)
   * @param output Result
   */
  void createSx(const ZZ_p &y, vector<ZZ_pX> &output);

  /**
   * @brief Update the commiment values (commitA, commitB, commitC, commitD) given by prover
   *
   * @param commits Commitment values
   */
  void setCommits(const Vec<ZZ_p> &commits);

  /**
   * @brief Calculate challenge value (y) base on commitment values, used in non-interactive mode
   *
   * @return ZZ_p
   */
  ZZ_p calculateY();

  /**
   * @brief Update the polynomial commitments result (pc)
   *
   * @param pc Polynomial commitments result
   */
  void setPolyCommits(const Vec<ZZ_p> &pc);

  /**
   * @brief Calculate challenge value (x) base on polynomial commitments result (pc)
   *
   * @return ZZ_p
   */
  ZZ_p calculateX();

  /**
   * @brief Verify the proofs for the circuit
   *
   * @param proofs The proofs list
   * @param y Challenge value y
   * @param x Challenge value x
   * @return true
   * @return false
   */
  bool verify(const Vec<ZZ_p> &proofs, const ZZ_p &y, const ZZ_p &x);
};

} // namespace polyu
