#pragma once

#include "../lib/namespace.hpp"

#include <stdexcept>
#include <vector>

#include <NTL/ZZ.h>
#include <NTL/ZZ_p.h>
#include <NTL/vector.h>
#include <NTL/matrix.h>

#include "./ConvertUtils.hpp"
#include "../lib/math/Matrix.hpp"

using namespace std;
using namespace NTL;

namespace polyu
{

class CBase
{
public:
  static void copyCircuit(const shared_ptr<CBase> &values, const shared_ptr<CBase> &target);

  ZZ GP_Q;
  ZZ GP_P;
  ZZ_p GP_G;

  vector<shared_ptr<Matrix>> Wqa;
  vector<shared_ptr<Matrix>> Wqb;
  vector<shared_ptr<Matrix>> Wqc;
  Vec<ZZ_p> Kq;

  shared_ptr<Matrix> A = nullptr;
  shared_ptr<Matrix> B = nullptr;
  shared_ptr<Matrix> C = nullptr;

  size_t linearCount = 0;
  size_t gateCount = 0;
  size_t offset = 0;

  explicit CBase();

  explicit CBase(
      const ZZ &GP_Q,
      const ZZ &GP_P,
      const ZZ_p &GP_G);

  explicit CBase(
      const ZZ &GP_Q,
      const ZZ &GP_P,
      const ZZ_p &GP_G,
      const vector<shared_ptr<Matrix>> &Wqa,
      const vector<shared_ptr<Matrix>> &Wqb,
      const vector<shared_ptr<Matrix>> &Wqc,
      const Vec<ZZ_p> &Kq,
      const shared_ptr<Matrix> &A = nullptr,
      const shared_ptr<Matrix> &B = nullptr,
      const shared_ptr<Matrix> &C = nullptr);

  void shift(size_t n);
  void group(size_t n, size_t m = 0);
  void trim();
  void append(const shared_ptr<CBase> &b);
  size_t assignValues(const shared_ptr<CBase> &b, size_t offset = 0);
  size_t addGate(size_t n = 1);
  size_t addLinear();

  json toJson();
  string toString();
};

} // namespace polyu
