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
  shared_ptr<Integer> N;
  vector<shared_ptr<Integer>> gi;

  PolynomialCommitment(
      const shared_ptr<Integer> &N,
      const vector<shared_ptr<Integer>> &gi);

  shared_ptr<Integer> commit(
      const vector<shared_ptr<Integer>> &mi,
      const shared_ptr<Integer> &r);

};

} // namespace polyu
