#pragma once

#include "../lib/namespace.hpp"

#include <stdexcept>
#include <vector>

#include "../lib/math/Random.hpp"
#include "../lib/math/IntegerImpl.hpp"
#include "../lib/math/Matrix.hpp"
#include "../lib/utils/HexUtils.hpp"

namespace polyu
{

class CircuitZKPVerifier
{
private:
public:
  shared_ptr<Integer> GP_Q;
  shared_ptr<Integer> GP_P;
  shared_ptr<Integer> GP_G;

  vector<shared_ptr<Matrix>> Wqa;
  vector<shared_ptr<Matrix>> Wqb;
  vector<shared_ptr<Matrix>> Wqc;
  vector<shared_ptr<Integer>> Kq;

  size_t N;
  size_t m;
  size_t n;
  size_t M;
  size_t Q;

  vector<shared_ptr<Integer>> commitA;
  vector<shared_ptr<Integer>> commitB;
  vector<shared_ptr<Integer>> commitC;
  shared_ptr<Integer> commitD;

  CircuitZKPVerifier(
      const shared_ptr<Integer> &GP_Q,
      const shared_ptr<Integer> &GP_P,
      const shared_ptr<Integer> &GP_G,
      const vector<shared_ptr<Matrix>> &Wqa,
      const vector<shared_ptr<Matrix>> &Wqb,
      const vector<shared_ptr<Matrix>> &Wqc,
      const vector<shared_ptr<Integer>> &Kq);

  void setCommits(const vector<shared_ptr<Integer>> &commits);
  void clearCommits();
  shared_ptr<Integer> calculateY();
};

} // namespace polyu
