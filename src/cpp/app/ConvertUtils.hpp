#pragma once

#include "../lib/namespace.hpp"

#include <fstream>
#include <stdexcept>
#include <vector>
#include <math.h>

#include <NTL/ZZ.h>
#include <NTL/ZZ_p.h>
#include <NTL/ZZ_pX.h>
#include <NTL/vector.h>

#include "../lib/math/IntegerImpl.hpp"
#include "../lib/math/Matrix.hpp"

using namespace std;
using namespace NTL;

namespace polyu
{

class ConvertUtils
{
public:
  static ZZ toZZ(const ZZ_p &input);
  static ZZ_p toZZ_p(const ZZ &input);

  static string toString(const ZZ &input);
  static string toString(const ZZ_p &input);

  static void toVecZZ(const Vec<ZZ_p> &input, Vec<ZZ> &output, size_t len = 0);
  static void toVecZZ_p(const Vec<ZZ> &input, Vec<ZZ_p> &output, size_t len = 0);
  static void subVec(const Vec<ZZ_p> &input, Vec<ZZ_p> &output, size_t start = 0, size_t end = 0);

  static ZZ toZZ(const binary_t &input);
  static ZZ_p toZZ_p(const binary_t &input);
  static void toVecZZ(const vector<binary_t> &input, Vec<ZZ> &output, size_t len = 0);
  static void toVecZZ_p(const vector<binary_t> &input, Vec<ZZ_p> &output, size_t len = 0);

  static binary_t hexToBinary(const string &input);
  static ZZ hexToZZ(const string &input);
  static ZZ_p hexToZZ_p(const string &input);

  static binary_t toBinary(const ZZ &input);
  static binary_t toBinary(const string &input);
  static string toString(const binary_t &input);

  static binary_t concat(const binary_t &a, const binary_t &b);
  static binary_t concat(const binary_t &a, const string &b);
  static binary_t concat(const string &a, const binary_t &b);
  static void append(binary_t &a, const binary_t &b);
  static binary_t padZero(const binary_t &data, const size_t length);
  static bool eq(const binary_t &a, const binary_t &b);

  // TODO: remove
  static ZZ toZZ(const shared_ptr<Integer> &input);
  static ZZ_p toZZ_p(const shared_ptr<Integer> &input);
  static void toVecZZ(const vector<shared_ptr<Integer>> &input, Vec<ZZ> &output, size_t len = 0);
  static void toVecZZ_p(const vector<shared_ptr<Integer>> &input, Vec<ZZ_p> &output, size_t len = 0);
};

} // namespace polyu

using namespace polyu;
