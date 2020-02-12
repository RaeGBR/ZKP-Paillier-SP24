#pragma once

#include "../lib/namespace.hpp"

#include <stdexcept>
#include <vector>

#include "../lib/math/Matrix.hpp"
#include "../lib/math/IntegerImpl.hpp"
#include "../lib/paillier/PaillierEncryption.hpp"
#include "./CBase.hpp"

using namespace std;

namespace polyu
{

class CEnc : public CBase
{
public:
  shared_ptr<Integer> N;

  using CBase::CBase;
  CEnc(const shared_ptr<PaillierEncryption> &crypto);

  void updateCipher(const shared_ptr<Integer> &C);
  void wireUp(const shared_ptr<Integer> &C = Integer::ZERO());
  void run(const shared_ptr<Integer> &m, const shared_ptr<Integer> &r);
};

} // namespace polyu
