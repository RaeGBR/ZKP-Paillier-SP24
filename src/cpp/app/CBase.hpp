#pragma once

#include "../lib/namespace.hpp"

#include <stdexcept>
#include <vector>

#include "../lib/math/Matrix.hpp"
#include "../lib/math/IntegerImpl.hpp"

using namespace std;

namespace polyu
{

class CBase
{
public:
  static void copyCircuit(const shared_ptr<CBase> &values, shared_ptr<CBase> &target);

  shared_ptr<Integer> GP_Q;
  shared_ptr<Integer> GP_P;
  shared_ptr<Integer> GP_G;

  vector<shared_ptr<Matrix>> Wqa;
  vector<shared_ptr<Matrix>> Wqb;
  vector<shared_ptr<Matrix>> Wqc;
  vector<shared_ptr<Integer>> Kq;

  shared_ptr<Matrix> A = nullptr;
  shared_ptr<Matrix> B = nullptr;
  shared_ptr<Matrix> C = nullptr;

  size_t linearCount = 0;
  size_t gateCount = 0;
  size_t offset = 0;

  explicit CBase();

  explicit CBase(
      const shared_ptr<Integer> &GP_Q,
      const shared_ptr<Integer> &GP_P,
      const shared_ptr<Integer> &GP_G,
      const vector<shared_ptr<Matrix>> &Wqa = vector<shared_ptr<Matrix>>(),
      const vector<shared_ptr<Matrix>> &Wqb = vector<shared_ptr<Matrix>>(),
      const vector<shared_ptr<Matrix>> &Wqc = vector<shared_ptr<Matrix>>(),
      const vector<shared_ptr<Integer>> &Kq = vector<shared_ptr<Integer>>(),
      const shared_ptr<Matrix> &A = nullptr,
      const shared_ptr<Matrix> &B = nullptr,
      const shared_ptr<Matrix> &C = nullptr);

  void shift(size_t n);
  void group(size_t n, size_t m = 0);
  void trim();
  size_t addGate(size_t n = 1);
  size_t addLinear();

  json toJson();
  string toString();
};

} // namespace polyu
