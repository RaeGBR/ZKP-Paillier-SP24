#pragma once

#include "../lib/namespace.hpp"

#include <stdexcept>
#include <vector>

#include "./CBase.hpp"
#include "./CEnc.hpp"
#include "./PaillierEncryption.hpp"
#include "./ConvertUtils.hpp"
#include "./MathUtils.hpp"

#include "../lib/math/Matrix.hpp"

using namespace std;

namespace polyu
{

class CBatchEnc : public CBase
{
public:
  shared_ptr<PaillierEncryption> crypto;

  size_t msgSize;         // msgSize = N's size = public key length
  size_t msgCount;        // msgCount
  size_t msgPerBatch;     // msgPerPatch
  size_t batchCount;      // batchCount = msgCount / msgPerBatch
  size_t slotSize;        // message's slot size (in byte)
  size_t slotsPerMsg;     //slotPerMsg * slotSize = msgSize
  size_t rangeProofCount; // rangeProofCount, number of constrains: l * b + Rj = Lj
  ZZ RjMax;               // 2 ^ * (slotsPerMsg * msgPerBatch / 3.4)

  Vec<ZZ> m;     // message;
  Vec<ZZ> m_;    // auxiliary message;
  Vec<ZZ_p> Rj;  // range proof mask
  Vec<ZZ_p> Rm;  // randomness of message;
  Vec<ZZ_p> Rm_; // randomness of auxiliary message;
  Vec<ZZ_p> RRj; // randomness of range proof mask
  Vec<ZZ_p> Cm;  // cipher of message;
  Vec<ZZ_p> Cm_; // cipher of auxiliary message;
  Vec<ZZ_p> CRj; // cipher of range proof mask

  using CBase::CBase;
  CBatchEnc(const shared_ptr<PaillierEncryption> &crypto,
            size_t msgCount, size_t rangeProofCount = 2,
            size_t slotSize = 4, size_t msgPerBatch = 15);

  void encrypt(const Vec<ZZ> &msg);
  void setCipher(const Vec<ZZ_p> &Cm,
                 const Vec<ZZ_p> &Cm_,
                 const Vec<ZZ_p> &CRj);
  size_t getLjLength();
  binary_t calculateLjir();
  Vec<ZZ_p> calculateLj(const binary_t &Ljir);

  void wireUp(const binary_t &Ljir, const Vec<ZZ_p> &Lj);
  void run(const binary_t &Ljir, const Vec<ZZ_p> &Lj);
};

} // namespace polyu
