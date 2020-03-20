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

  // static shared_ptr<Matrix> powerVector(const shared_ptr<Integer> &x,
  //                                       size_t n,
  //                                       const shared_ptr<Integer> &modulus = Integer::ZERO()); // [1, x, x^2, ..., x^(n-1)];

  vector<map<size_t, ZZ_p>> values;
  size_t m;
  size_t n;

  Matrix();                          // [[0]]
  Matrix(size_t m, size_t n);        // All zero matrix
  Matrix(const vector<int> &values); // Initialized with defaults
  // Matrix(const vector<shared_ptr<Integer>> &values);         // Initialized with defaults
  // Matrix(const vector<vector<shared_ptr<Integer>>> &values); // Initialized with defaults

  shared_ptr<Matrix> clone();

  bool cellExists(size_t i, size_t j);
  ZZ_p cell(size_t i, size_t j);
  void cell(size_t i, size_t j, long x);
  void cell(size_t i, size_t j, const ZZ_p &x);

  bool rowExists(size_t i);
  // shared_ptr<Matrix> rowAsMatrix(size_t i);
  // void row(size_t i, vector<shared_ptr<Integer>> &result);
  void row(size_t i, Vec<ZZ_p> &output);
  void toMat(Mat<ZZ_p> &output);

  // void appendRow(const vector<shared_ptr<Integer>> &row);
  // void appendCol(const vector<shared_ptr<Integer>> &col);

  shared_ptr<Matrix> group(size_t n, size_t m = 0);

  // shared_ptr<Matrix> t();
  // shared_ptr<Matrix> add(const shared_ptr<Matrix> &b, const shared_ptr<Integer> &modulus = Integer::ZERO());
  // shared_ptr<Matrix> mul(const shared_ptr<Integer> &b, const shared_ptr<Integer> &modulus = Integer::ZERO());
  // shared_ptr<Matrix> mul(const shared_ptr<Matrix> &b, const shared_ptr<Integer> &modulus = Integer::ZERO());
  // shared_ptr<Matrix> inner(const shared_ptr<Matrix> &b, const shared_ptr<Integer> &modulus = Integer::ZERO());
  // shared_ptr<Matrix> dot(const shared_ptr<Matrix> &b, const shared_ptr<Integer> &modulus = Integer::ZERO());

  void shift(size_t n);
  void extend(size_t n);
  void trim(); // trim all zero cells

  bool eq(const shared_ptr<Matrix> &b);

  json toJson();
  string toString();
};

} // namespace cryptoplus
