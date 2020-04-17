#pragma once

#include "./namespace.hpp"

#include <NTL/ZZ.h>
#include <NTL/ZZ_p.h>
#include <NTL/vector.h>
#include <NTL/matrix.h>

#include "./utils/ConvertUtils.hpp"
#include "./CircuitZKPVerifier.hpp"
#include "./CircuitZKPProver.hpp"
#include "./math/Matrix.hpp"

namespace polyu
{

class CBase
{
private:
  void convertWire(const vector<shared_ptr<Matrix>> &source,
                   map<size_t, map<size_t, map<size_t, ZZ_p>>> &target,
                   size_t m, size_t n);

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

  shared_ptr<CircuitZKPVerifier> generateVerifier(const Vec<ZZ_p> &gi);
  shared_ptr<CircuitZKPProver> generateProver(const Vec<ZZ_p> &gi);

  json toJson();
  string toString();
};

} // namespace polyu
