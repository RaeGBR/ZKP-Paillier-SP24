#pragma once

#include "../namespace.hpp"

#include <NTL/ZZ.h>
#include <NTL/ZZ_p.h>
#include <NTL/vector.h>

namespace polyu
{

class MathUtils
{
public:
  /**
   * @brief Random a big interger under a modular group
   *
   * @param modulus The modulus
   * @param positiveOnly Positive only flag
   * @return ZZ_p
   */
  static ZZ_p randZZ_p(const ZZ &modulus, bool positiveOnly = false);

  /**
   * @brief Random a list of big intergers under a modular group
   *
   * @param n
   * @param modulus
   * @param ret Result
   * @param positiveOnly Positive only flag
   */
  static void randVecZZ_p(size_t n, const ZZ &modulus, Vec<ZZ_p> &ret, bool positiveOnly = false);

  /**
   * @brief Generate a power sequence [1, x, x^2, x^3, ..., x^(n-1)]
   *
   * @param xn Power base (x)
   * @param n
   * @param modulus
   * @param ret Result
   */
  static void powerVecZZ_p(const ZZ_p &xn, size_t n, const ZZ &modulus, Vec<ZZ_p> &ret);
};

} // namespace polyu
