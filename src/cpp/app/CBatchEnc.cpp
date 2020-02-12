#include "./CBatchEnc.hpp"

using namespace polyu;

CBatchEnc::CBatchEnc(const shared_ptr<PaillierEncryption> &crypto,
                     size_t msgCount, size_t rangeProofCount,
                     size_t slotSize, size_t msgPerBatch)
{
  this->GP_Q = crypto->getGroupQ();
  this->GP_P = crypto->getGroupP();
  this->GP_G = crypto->getGroupG();

  this->crypto = crypto;
  this->msgCount = msgCount;
  this->rangeProofCount = rangeProofCount;
  this->slotSize = slotSize;
  this->msgPerBatch = msgPerBatch;

  this->msgSize = crypto->getPublicKey()->toBinary().size();
  this->batchCount = (size_t)ceil(msgCount * 1.0 / msgPerBatch);
  this->slotsPerMsg = (size_t)floor(msgSize * 1.0 / slotSize);
  auto RjPow = make_shared<IntegerImpl>((int)round(slotsPerMsg * msgPerBatch / 3.4));
  this->RjMax = Integer::TWO()->pow(RjPow);
}

void CBatchEnc::encrypt(const vector<shared_ptr<Integer>> &msg)
{
  if (msg.size() != msgCount)
    throw invalid_argument("number of messages do not match with the configure");

  m.clear();
  m_.clear();
  Rj.clear();
  Rm.clear();
  Rm_.clear();
  RRj.clear();
  Cm.clear();
  Cm_.clear();
  CRj.clear();

  m.insert(m.begin(), msg.begin(), msg.end());
  for (size_t i = 0; i < msgCount; i++)
  {
    auto r = crypto->pickRandom();
    auto c = crypto->encrypt(m[i], r);
    Rm.push_back(r);
    Cm.push_back(c);
  }
  for (size_t i = 0; i < rangeProofCount; i++)
  {
    auto rj = Random::genInteger(RjMax);
    auto r = crypto->pickRandom();
    auto c = crypto->encrypt(rj, r);
    Rj.push_back(rj);
    RRj.push_back(r);
    CRj.push_back(c);
  }
  for (size_t i = 0; i < batchCount; i++)
  {
    string m_iStr = "";
    for (size_t j = 0; j < msgPerBatch; j++)
    {
      auto idx = i * msgPerBatch + j;
      if (idx >= msgCount)
        break;

      auto mi = m[idx]->toFixedBinary(msgSize);
      string miStr = "";
      for (size_t k = 0; k < slotsPerMsg; k++)
      {
        auto bi = mi[(k + 1) * slotSize - 1];
        miStr += bi == 0 ? "0" : "1";
      }
      m_iStr = miStr + m_iStr;
    }
    auto m_i = make_shared<IntegerImpl>(m_iStr.c_str(), 2);
    auto r = crypto->pickRandom();
    auto c = crypto->encrypt(m_i, r);
    m_.push_back(m_i);
    Rm_.push_back(r);
    Cm_.push_back(c);
  }
}

void CBatchEnc::setCipher(const vector<shared_ptr<Integer>> &Cm,
                          const vector<shared_ptr<Integer>> &Cm_,
                          const vector<shared_ptr<Integer>> &CRj)
{
  if (Cm.size() != msgCount || Cm_.size() != batchCount || CRj.size() != rangeProofCount)
    throw invalid_argument("cipher count do not match with settings");

  this->Cm.clear();
  this->Cm_.clear();
  this->CRj.clear();
  this->Cm.insert(this->Cm.begin(), Cm.begin(), Cm.end());
  this->Cm_.insert(this->Cm_.begin(), Cm_.begin(), Cm_.end());
  this->CRj.insert(this->CRj.begin(), CRj.begin(), CRj.end());
}

size_t CBatchEnc::getLjLength()
{
  return (size_t)ceil((msgCount * 1.0 * slotsPerMsg * rangeProofCount) / 8.0);
}

binary_t CBatchEnc::calculateLj()
{
  size_t len = getLjLength();
  binary_t seed;
  for (auto v : Cm)
    HexUtils::append(seed, v->toBinary());
  for (auto v : Cm_)
    HexUtils::append(seed, v->toBinary());
  for (auto v : CRj)
    HexUtils::append(seed, v->toBinary());

  return Random::genBinary(len, seed);
}

void CBatchEnc::wireUp(const binary_t &Lj)
{
  auto encCir = make_shared<CEnc>(crypto);
  encCir->wireUp();
  auto encCirN = encCir->gateCount;
  auto encCirQ = encCir->linearCount;

  // Const{ci, mi, ri} x k
  // A[0][0] = m
  // A[0][1] = r
  // C[0][q-1] = c
  size_t encMiOffset = 0;
  for (size_t i = 0; i < msgCount; i++)
  {
    auto cir = encCir;
    auto c = this->Cm[i];
    cir->updateCipher(c);
    this->append(cir);
  }

  // Const{c'j, R'j, r'j} x j
  size_t encRjOffset = gateCount;
  for (size_t i = 0; i < rangeProofCount; i++)
  {
    auto cir = encCir;
    auto c = this->CRj[i];
    cir->updateCipher(c);
    this->append(cir);
  }

  // Const{c*s, m*s, r*s} x s
  size_t encM_Offset = gateCount;
  for (size_t i = 0; i < batchCount; i++)
  {
    auto cir = encCir;
    auto c = this->Cm_[i];
    cir->updateCipher(c);
    this->append(cir);
  }
}

void CBatchEnc::run(const binary_t &Lj)
{
  auto encCir = make_shared<CEnc>(crypto);
  encCir->wireUp();
  auto encCirN = encCir->gateCount;
  auto encCirQ = encCir->linearCount;

  size_t offset = 0;

  // Const{ci, mi, ri} x k
  // A[0][0] = m
  // A[0][1] = r
  // C[0][q-1] = c
  for (size_t i = 0; i < msgCount; i++)
  {
    auto cir = encCir;
    auto c = this->Cm[i];
    auto m = this->m[i];
    auto r = this->Rm[i];
    cir->updateCipher(c);
    cir->run(m, r);
    offset = this->assignValues(cir, offset);
  }

  // Const{c'j, R'j, r'j} x j
  size_t encRjOffset = gateCount;
  for (size_t i = 0; i < rangeProofCount; i++)
  {
    auto cir = encCir;
    auto c = this->CRj[i];
    auto m = this->Rj[i];
    auto r = this->RRj[i];
    cir->updateCipher(c);
    cir->run(m, r);
    offset = this->assignValues(cir, offset);
  }

  // Const{c*s, m*s, r*s} x s
  size_t encM_Offset = gateCount;
  for (size_t i = 0; i < batchCount; i++)
  {
    auto cir = encCir;
    auto c = this->Cm_[i];
    auto m = this->m_[i];
    auto r = this->Rm_[i];
    cir->updateCipher(c);
    cir->run(m, r);
    offset = this->assignValues(cir, offset);
  }
}
