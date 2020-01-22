#pragma once

#include "../namespace.hpp"

#include <stdexcept>
#include <vector>

#include "Integer.hpp"
#include "./IntegerImpl.hpp"

using namespace std;

namespace cryptoplus
{

class Matrix
{
public:
  static shared_ptr<Matrix> identity(size_t size); // Identity matrix
  static shared_ptr<Matrix> powerVector(const shared_ptr<Integer> &x,
                                        size_t n,
                                        const shared_ptr<Integer> &modulus = Integer::ZERO()); // [1, x, x^2, ..., x^(n-1)];

  vector<vector<shared_ptr<Integer>>> values;
  size_t m;
  size_t n;

  Matrix(size_t m, size_t n);                                // All zero matrix
  Matrix(const vector<int> &values);                         // Initialized with defaults
  Matrix(const vector<shared_ptr<Integer>> &values);         // Initialized with defaults
  Matrix(const vector<vector<shared_ptr<Integer>>> &values); // Initialized with defaults

  shared_ptr<Matrix> t();
  shared_ptr<Matrix> add(const shared_ptr<Matrix> &b, const shared_ptr<Integer> &modulus = Integer::ZERO());
  shared_ptr<Matrix> mul(const shared_ptr<Integer> &b, const shared_ptr<Integer> &modulus = Integer::ZERO());
  shared_ptr<Matrix> mul(const shared_ptr<Matrix> &b, const shared_ptr<Integer> &modulus = Integer::ZERO());
  shared_ptr<Matrix> dot(const shared_ptr<Matrix> &b, const shared_ptr<Integer> &modulus = Integer::ZERO());
  void appendRow(const vector<shared_ptr<Integer>> &row);
  void appendCol(const vector<shared_ptr<Integer>> &col);

  shared_ptr<Matrix> clone();
  shared_ptr<Matrix> group(size_t n, size_t m = 0);
  bool eq(const shared_ptr<Matrix> &b);

  json toJson();
  string toString();

  vector<shared_ptr<Integer>> &operator[](size_t idx)
  {
    return values[idx];
  }
};

} // namespace cryptoplus
