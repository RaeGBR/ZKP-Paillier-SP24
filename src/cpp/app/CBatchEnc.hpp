#pragma once

#include "../lib/namespace.hpp"

#include <stdexcept>
#include <vector>

#include "../lib/math/Matrix.hpp"
#include "../lib/math/IntegerImpl.hpp"
#include "../lib/paillier/PaillierEncryption.hpp"
#include "../lib/utils/HexUtils.hpp"
#include "./CBase.hpp"
#include "./CEnc.hpp"

using namespace std;

namespace polyu
{

class CBatchEnc : public CBase
{
public:
  shared_ptr<PaillierEncryption> crypto;

  size_t msgSize;            // msgSize = N's size = public key length
  size_t msgCount;           // msgCount
  size_t msgPerBatch;        // msgPerPatch
  size_t batchCount;         // batchCount = msgCount / msgPerBatch
  size_t slotSize;           // message's slot size (in byte)
  size_t slotsPerMsg;        //slotPerMsg * slotSize = msgSize
  size_t rangeProofCount;    // rangeProofCount, number of constrains: l * b + Rj = Lj
  shared_ptr<Integer> RjMax; // 2 ^ * (slotsPerMsg * msgPerBatch / 3.4)

  vector<shared_ptr<Integer>> m;   // message;
  vector<shared_ptr<Integer>> m_;  // auxiliary message;
  vector<shared_ptr<Integer>> Rj;  // range proof mask
  vector<shared_ptr<Integer>> Rm;  // randomness of message;
  vector<shared_ptr<Integer>> Rm_; // randomness of auxiliary message;
  vector<shared_ptr<Integer>> RRj; // randomness of range proof mask
  vector<shared_ptr<Integer>> Cm;  // cipher of message;
  vector<shared_ptr<Integer>> Cm_; // cipher of auxiliary message;
  vector<shared_ptr<Integer>> CRj; // cipher of range proof mask

  using CBase::CBase;
  CBatchEnc(const shared_ptr<PaillierEncryption> &crypto,
            size_t msgCount, size_t rangeProofCount = 2,
            size_t slotSize = 4, size_t msgPerBatch = 15);

  void encrypt(const vector<shared_ptr<Integer>> &msg);
  void setCipher(const vector<shared_ptr<Integer>> &Cm,
                 const vector<shared_ptr<Integer>> &Cm_,
                 const vector<shared_ptr<Integer>> &CRj);
  size_t getLjLength();
  binary_t calculateLjir();
  vector<shared_ptr<Integer>> calculateLj(const binary_t &Ljir);

  void wireUp(const binary_t &Ljir, const vector<shared_ptr<Integer>> &Lj);
  void run(const binary_t &Ljir, const vector<shared_ptr<Integer>> &Lj);
};

} // namespace polyu
