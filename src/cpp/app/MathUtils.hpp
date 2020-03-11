#pragma once

#include "../lib/namespace.hpp"

#include <vector>

#include <NTL/ZZ.h>
#include <NTL/ZZ_p.h>
#include <NTL/vector.h>

using namespace std;
using namespace NTL;

namespace polyu
{

class MathUtils
{
public:
  static ZZ_p randZZ_p(const ZZ &modulus, bool positiveOnly = false);
  static void randVecZZ_p(size_t n, const ZZ &modulus, Vec<ZZ_p> &ret, bool positiveOnly = false);
  static void powerVecZZ_p(const ZZ_p &xn, size_t n, const ZZ &modulus, Vec<ZZ_p> &ret); // [1, x, x^2, x^3, ...]
};

} // namespace polyu

using namespace polyu;
