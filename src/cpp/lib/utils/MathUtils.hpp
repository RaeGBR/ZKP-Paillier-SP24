#pragma once

#include <math.h>
#include <string.h>

#include "Integer.hpp"
#include "../wrappers/IntegerImpl.hpp"

using namespace std;
using namespace cryptoplus;

namespace cryptoplus
{

class MathUtils
{
public:
  // 2^(bits-1) + x
  static shared_ptr<Integer> encodeComplement(shared_ptr<Integer> x, int bits);

  // x - 2^(bits-1)
  static shared_ptr<Integer> decodeComplement(shared_ptr<Integer> x, int bits);

  // Decimal Zero
  static shared_ptr<Integer> decimalZero(unsigned long precision);

  // 2^(precision * 4) + { input * e^(scale) } <= To Integer
  static shared_ptr<Integer> encodeDecimal(string x, unsigned long precision, unsigned long scale);

  // (x - 2^(precision * 4)) to Scale
  static string decodeDecimal(shared_ptr<Integer> x, unsigned long precision, unsigned long scale);
};

} // namespace cryptoplus
