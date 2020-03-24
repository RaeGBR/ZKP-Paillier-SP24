#pragma once

#include "../namespace.hpp"

#include <stdexcept>
#include <map>
#include <vector>

#include <NTL/ZZ.h>
#include <NTL/ZZ_p.h>
#include <NTL/vector.h>
#include <NTL/matrix.h>

#include "../../app/ConvertUtils.hpp"

// #include "Integer.hpp"
// #include "./IntegerImpl.hpp"

using namespace std;
using namespace NTL;
using namespace polyu;

namespace cryptoplus
{

class Matrix
{
public:
  static shared_ptr<Matrix> ZERO();

  vector<map<size_t, ZZ_p>> values;
  size_t m;
  size_t n;

  Matrix();                          // [[0]]
  Matrix(size_t m, size_t n);        // All zero matrix
  Matrix(const vector<int> &values); // Initialized with defaults

  shared_ptr<Matrix> clone();

  bool cellExists(size_t i, size_t j);
  ZZ_p cell(size_t i, size_t j);
  void cell(size_t i, size_t j, long x);
  void cell(size_t i, size_t j, const ZZ_p &x);

  bool rowExists(size_t i);
  void row(size_t i, Vec<ZZ_p> &output);
  void toMat(Mat<ZZ_p> &output);

  shared_ptr<Matrix> group(size_t n, size_t m = 0);

  void shift(size_t n);
  void extend(size_t n);
  void trim(); // trim all zero cells

  bool eq(const shared_ptr<Matrix> &b);

  json toJson();
  string toString();
};

} // namespace cryptoplus
