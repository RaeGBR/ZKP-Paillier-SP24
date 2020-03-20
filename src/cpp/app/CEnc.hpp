#pragma once

#include "../lib/namespace.hpp"

#include <stdexcept>
#include <vector>

#include "./PaillierEncryption.hpp"
#include "./CBase.hpp"

#include "../lib/math/Matrix.hpp"

using namespace std;

namespace polyu
{

class CEnc : public CBase
{
public:
  ZZ N;

  using CBase::CBase;
  CEnc(const shared_ptr<PaillierEncryption> &crypto);

  void updateCipher(const ZZ_p &C);
  void wireUp(const ZZ_p &C = ZZ_p());
  void run(const ZZ &m, const ZZ_p &r);
};

} // namespace polyu
