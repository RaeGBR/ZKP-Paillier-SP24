#pragma once

#include "./namespace.hpp"

#include <NTL/vector.h>
#include <NTL/matrix.h>
#include <NTL/ZZ.h>
#include <NTL/ZZ_p.h>
#include <NTL/ZZ_pX.h>

#include "./PaillierEncryption.hpp"
#include "./utils/ConvertUtils.hpp"
#include "./math/MathUtils.hpp"

namespace polyu
{

/**
 * @brief _PolynomialCommitment_ handles the commitment scheme for polynomial. It requires the a _PaillierEncryption_ group and a list of independent generators as a setup parameters.
 */
class PolynomialCommitment
{
public:
  /**
   * @brief Group element Q
   */
  ZZ Q; // Group order

  /**
   * @brief Group element p, (Q-1) % p = 0
   */
  ZZ p;

  /**
   * @brief Base group generator g, (g^p) % Q = 1
   */
  ZZ_p g;

  /**
   * @brief Independent generators for commitment [g0, g1, ... , gn]
   */
  Vec<ZZ_p> gi;

  /**
   * @brief Construct a new polynomial commitment scheme
   *
   * @param crypto PaillierEncryption object, defined the group elements
   * @param n Degree of polynomial / number of generators
   */
  PolynomialCommitment(const shared_ptr<PaillierEncryption> &crypto, size_t n);

  /**
   * @brief Construct a new polynomial commitment scheme
   *
   * @param crypto PaillierEncryption object, defined the group elements
   * @param gi Pre-defined generators
   */
  PolynomialCommitment(const shared_ptr<PaillierEncryption> &crypto, const Vec<ZZ_p> &gi);

  /**
   * @brief Construct a new polynomial commitment scheme
   *
   * @param Q Group element Q
   * @param p Group element p
   * @param g Base group generator g
   * @param gi Pre-defined generators
   */
  PolynomialCommitment(const ZZ &Q, const ZZ &p, const ZZ_p &g, const Vec<ZZ_p> &gi);

  /**
   * @brief (DEPRECATED) Construct a new polynomial commitment scheme, the generators gi are insecure
   * @deprecated
   *
   * @param Q Group element Q
   * @param p Group element p
   * @param g Base group generator g
   * @param n Degree of polynomial / number of generators
   */
  PolynomialCommitment(const ZZ &Q, const ZZ &p, const ZZ_p &g, size_t n);

  /**
   * @brief Commit a message
   *
   * @param mi Message (m)
   * @param r Randomness (r)
   * @return ZZ_p Commitment (c)
   */
  ZZ_p commit(const Vec<ZZ_p> &mi, const ZZ_p &r);

  /**
   * @brief Commit multiple messages
   *
   * @param ms Messages (ms)
   * @param rs Randomness (rs)
   * @param ret Commitments result
   */
  void commit(const Mat<ZZ_p> &ms, const Vec<ZZ_p> &rs, Vec<ZZ_p> &ret);

  /**
   * @brief Calculate matrix (T) for polynomial commitment
   *
   * @param m1
   * @param m2
   * @param n
   * @param tx Polynomial t(x)
   * @param ret Result matrix (T)
   */
  void calcT(
      size_t m1, size_t m2, size_t n,
      const ZZ_pX &tx, Mat<ZZ_p> &ret);

  /**
   * @brief Calculate polynomial commitment
   *
   * @param m1
   * @param m2
   * @param n
   * @param T Matrix (T)
   * @param ri Randomness (r_i)
   * @param ret Commitments result (pc)
   */
  void commit(
      size_t m1, size_t m2, size_t n,
      const Mat<ZZ_p> &T, Vec<ZZ_p> &ri, Vec<ZZ_p> &ret);

  /**
   * @brief Calculate polynomial evaluate
   *
   * @param m1
   * @param m2
   * @param n
   * @param T Matrix (T)
   * @param ri Randomness (r_i)
   * @param x Challenge value (x)
   * @param result Evaluate result (pe)
   */
  void eval(
      size_t m1, size_t m2, size_t n,
      const Mat<ZZ_p> &T, const Vec<ZZ_p> &ri, const ZZ_p &x,
      Vec<ZZ_p> &result);

  /**
   * @brief Verify polynomial commitments
   *
   * @param m1
   * @param m2
   * @param n
   * @param pc Commitments result (pc)
   * @param pe Evaluate result (pe)
   * @param x Challenge value (x)
   * @return true
   * @return false
   */
  bool verify(
      size_t m1, size_t m2, size_t n,
      const Vec<ZZ_p> &pc, const Vec<ZZ_p> &pe, const ZZ_p &x);

  /**
   * @brief Calculate v = t(x)
   *
   * @param n
   * @param pe Evaluate result (pe)
   * @param x Challenge value (x)
   * @return ZZ_p v = t(x)
   */
  ZZ_p calcV(size_t n, const Vec<ZZ_p> &pe, const ZZ_p &x);
};

} // namespace polyu
