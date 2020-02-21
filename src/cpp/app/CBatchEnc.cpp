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
        auto r = slotsPerMsg - k - 1;
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

binary_t CBatchEnc::calculateLjir()
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

vector<shared_ptr<Integer>> CBatchEnc::calculateLj(const binary_t &Ljir)
{
  vector<shared_ptr<Integer>> Lj;

  auto two32 = Integer::TWO()->modPow(make_shared<IntegerImpl>(slotSize * 8), GP_P);
  vector<shared_ptr<Integer>> two32s; // [1, 2^32, 2^64, ... , 2^(32 * (r-1))]
  Matrix::powerVector(two32, slotsPerMsg, GP_P)->row(0, two32s);

  // SUM( lj * bri ) + R'j = L'j
  for (size_t j = 0; j < rangeProofCount; j++)
  {
    auto lj = Rj[j]; // R'j

    for (size_t i = 0; i < msgCount; i++)
    {
      auto mi = m[i]->toFixedBinary(msgSize);
      size_t ji = j * msgCount * slotsPerMsg + i * slotsPerMsg;
      for (size_t k = 0; k < slotsPerMsg; k++)
      {
        auto r = slotsPerMsg - k - 1;
        auto bi = mi[(k + 1) * slotSize - 1];
        if (bi == 1)
        {
          auto jir = ji + r;
          auto idx1 = jir & 7;  // mod 8
          auto idx2 = jir >> 3; // div 8
          auto ljir = (Ljir[idx2] >> idx1) & 1;
          if (ljir == 1)
            lj = lj->add(Integer::ONE()); // lj * bri
        }
      }
    }
    Lj.push_back(lj);
  }
  return Lj;
}

void CBatchEnc::wireUp(const binary_t &Ljir, const vector<shared_ptr<Integer>> &Lj)
{
  auto encCir = make_shared<CEnc>(crypto);
  encCir->wireUp();
  auto encCirN = encCir->gateCount;
  auto encCirQ = encCir->linearCount;

  // Const{ci, mi, ri} x k
  // A[0][0] = m
  // A[0][1] = r
  // C[0][q-1] = c
  size_t encMOffset = 0;
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

  size_t n, q;
  auto ZERO = Integer::ZERO();
  auto ONE = Integer::ONE();
  auto NEG_ONE = make_shared<IntegerImpl>(-1);

  // bri * (bri - 1) = 0
  size_t briOffset = gateCount;
  for (size_t i = 0; i < msgCount; i++)
  {
    for (size_t k = 0; k < slotsPerMsg; k++)
    {
      auto r = slotsPerMsg - k - 1;
      // gate: bri * (bri - 1) = 0
      n = addGate();

      // linear: ai - bi = 1;
      q = addLinear();
      Wqa[q - 1]->cell(0, n - 1, ONE);
      Wqb[q - 1]->cell(0, n - 1, NEG_ONE);
      Kq[q - 1] = ONE;

      // linear: ci = 0;
      q = addLinear();
      Wqc[q - 1]->cell(0, n - 1, ONE);
    }
  }

  auto two32 = Integer::TWO()->modPow(make_shared<IntegerImpl>(slotSize * 8), GP_P);
  vector<shared_ptr<Integer>> two32s; // [1, 2^32, 2^64, ... , 2^(32 * (r-1))]
  Matrix::powerVector(two32, slotsPerMsg, GP_P)->row(0, two32s);
  // linear: SUM( 2^(32 * r)) * bri ) = mi
  //         SUM - mi = 0
  for (size_t i = 0; i < msgCount; i++)
  {
    q = addLinear();

    // -mi
    Wqa[q - 1]->cell(0, encMOffset + encCirN * i, NEG_ONE);

    auto bIdx = i * slotsPerMsg;
    auto offset = briOffset + bIdx;
    for (size_t k = 0; k < slotsPerMsg; k++)
    {
      // 2^(32 * r) * bri
      auto r = slotsPerMsg - k - 1;
      Wqa[q - 1]->cell(0, offset + r, two32s[r]);
    }
  }

  // linear: SUM( lj * bri ) + R'j = L'j
  for (size_t j = 0; j < rangeProofCount; j++)
  {
    q = addLinear();

    // R'j
    Wqa[q - 1]->cell(0, encRjOffset + encCirN * j, ONE);

    for (size_t i = 0; i < msgCount; i++)
    {
      size_t ji = j * msgCount * slotsPerMsg + i * slotsPerMsg;
      for (size_t k = 0; k < slotsPerMsg; k++)
      {
        auto r = slotsPerMsg - k - 1;
        auto jir = ji + r;
        auto idx1 = jir & 7;  // mod 8
        auto idx2 = jir >> 3; // div 8
        auto ljir = (Ljir[idx2] >> idx1) & 1;
        if (ljir == 1)
        {
          auto bIdx = i * slotsPerMsg;
          auto offset = briOffset + bIdx;
          Wqa[q - 1]->cell(0, offset + r, ONE); // li * bri
        }
      }
    }

    Kq[q - 1] = Lj[j]; // L'j
  }

  // linear: 2^959 * bri + ... 2^0 * bri = m*s
  //         2^959 * bri + ... 2^0 * bri - m*s = 0
  vector<shared_ptr<Integer>> TWOs;
  Matrix::powerVector(Integer::TWO(), slotsPerMsg * msgPerBatch, GP_P)->row(0, TWOs);
  for (size_t s = 0; s < batchCount; s++)
  {
    q = addLinear();

    // -m*s
    Wqa[q - 1]->cell(0, encM_Offset + encCirN * s, NEG_ONE);

    for (size_t j = 0; j < msgPerBatch; j++)
    {
      auto i = s * msgPerBatch + j; // the last batch may not fully filled
      if (i >= msgCount)
        break;

      auto bIdx = i * slotsPerMsg;
      auto offset = briOffset + bIdx;
      auto twoOffset = j * slotsPerMsg;
      for (size_t r = 0; r < slotsPerMsg; r++)
      {
        auto pow = TWOs[twoOffset + r];
        Wqa[q - 1]->cell(0, offset + r, pow); // 2^959 * bri
      }
    }
  }
}

void CBatchEnc::run(const binary_t &Ljir, const vector<shared_ptr<Integer>> &Lj)
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
  size_t encMOffset = 0;
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
  size_t encRjOffset = offset;
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
  size_t encM_Offset = offset;
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

  auto ONE = Integer::ONE();
  auto ZERO = Integer::ZERO();
  auto NEG_ONE = ZERO->sub(ONE)->mod(GP_P);

  // bri * (bri - 1) = 0
  size_t briOffset = offset;
  for (size_t i = 0; i < msgCount; i++)
  {
    auto bIdx = i * slotsPerMsg;
    offset = briOffset + bIdx;
    auto mi = m[i]->toFixedBinary(msgSize);
    for (size_t k = 0; k < slotsPerMsg; k++)
    {
      // gate: bri * (bri - 1) = 0
      auto r = slotsPerMsg - k - 1;
      auto bi = mi[(k + 1) * slotSize - 1];
      A->cell(0, offset + r, bi == 0 ? ZERO : ONE);
      B->cell(0, offset + r, bi == 0 ? NEG_ONE : ZERO);
    }
  }
}
