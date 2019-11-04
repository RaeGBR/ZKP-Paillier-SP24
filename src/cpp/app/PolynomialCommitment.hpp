#pragma once

#include "../lib/namespace.hpp"

#include <stdexcept>
#include <vector>

#include "Integer.hpp"
#include "../lib/math/Matrix.hpp"
#include "../lib/math/Random.hpp"
#include "../lib/utils/HexUtils.hpp"

namespace polyu
{

class PolynomialCommitment
{
public:
  shared_ptr<Integer> Q;          // Group order
  shared_ptr<Integer> p;          // sub-group order, (Q-1) % p = 0
  shared_ptr<Integer> g;          // base generator, (g^p) % Q = 1
  vector<shared_ptr<Integer>> gi; // g, g^2, g^3, ...

  PolynomialCommitment(
      const shared_ptr<Integer> &Q,
      const shared_ptr<Integer> &p,
      const shared_ptr<Integer> &g,
      size_t n);

  shared_ptr<Integer> commit(
      const vector<shared_ptr<Integer>> &mi,
      const shared_ptr<Integer> &r);

  shared_ptr<Matrix> calcT(
      size_t m1, size_t m2, size_t n,
      const vector<shared_ptr<Integer>> &tx);

  vector<shared_ptr<Integer>> commit(
      size_t m1, size_t m2, size_t n,
      const shared_ptr<Matrix> &T,
      vector<shared_ptr<Integer>> &ri);

  vector<shared_ptr<Integer>> eval(
      size_t m1, size_t m2, size_t n,
      const shared_ptr<Matrix> &T,
      const vector<shared_ptr<Integer>> &ri,
      const shared_ptr<Integer> &x);

  bool verify(
      size_t m1, size_t m2, size_t n,
      const vector<shared_ptr<Integer>> &pc,
      const vector<shared_ptr<Integer>> &pe,
      const shared_ptr<Integer> &x);

  shared_ptr<Integer> calcV(
      size_t n,
      const vector<shared_ptr<Integer>> &pe,
      const shared_ptr<Integer> &x);
};

} // namespace polyu
