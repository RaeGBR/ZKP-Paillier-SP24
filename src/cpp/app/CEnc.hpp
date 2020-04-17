#pragma once

#include "./namespace.hpp"

#include "./PaillierEncryption.hpp"
#include "./CBase.hpp"

#include "./math/Matrix.hpp"

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
