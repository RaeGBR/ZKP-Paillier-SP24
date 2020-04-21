#include "./CBatchEnc.hpp"

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

  this->msgSize = NumBytes(crypto->getPublicKey());
  this->batchCount = (size_t)ceil(msgCount * 1.0 / msgPerBatch);
  this->slotsPerMsg = (size_t)floor(msgSize * 1.0 / slotSize);
  long RjPow = round(slotsPerMsg * msgPerBatch / 3.4);
  this->RjMax = conv<ZZ>(2) << (RjPow - 1); // 2^rRjPow
}

size_t CBatchEnc::estimateGateCount()
{
  auto encCir = make_shared<CEnc>(crypto);
  encCir->wireUp();
  return encCir->gateCount * (msgCount + rangeProofCount + batchCount) + (msgCount * slotsPerMsg);
}

size_t CBatchEnc::estimateGeneratorsRequired()
{
  auto N = estimateGateCount();
  auto mnCfg = CircuitZKPVerifier::calcMN(N);
  auto m = mnCfg[0];
  auto n = mnCfg[1];
  auto txCfg = CircuitZKPVerifier::calcM1M2N(m);
  auto txM1 = txCfg[0];
  auto txM2 = txCfg[1];
  auto txN = txCfg[2];
  auto ret = max(txN, n);
  return ret;
}

void CBatchEnc::encrypt(const Vec<ZZ> &msg)
{
  if (msg.length() != msgCount)
    throw invalid_argument("number of messages do not match with the configure");

  m.SetLength(0);
  m_.SetLength(0);
  Rj.SetLength(0);
  Rm.SetLength(0);
  Rm_.SetLength(0);
  RRj.SetLength(0);
  Cm.SetLength(0);
  Cm_.SetLength(0);
  CRj.SetLength(0);

  m = msg;

  for (size_t i = 0; i < msgCount; i++)
  {
    auto r = crypto->pickRandom();
    auto c = crypto->encrypt(m[i], r);
    Rm.append(r);
    Cm.append(c);
  }
  for (size_t i = 0; i < rangeProofCount; i++)
  {
    auto rj = MathUtils::randZZ_p(RjMax);
    auto r = crypto->pickRandom();
    auto c = crypto->encrypt(conv<ZZ>(rj), r);
    Rj.append(rj);
    RRj.append(r);
    CRj.append(c);
  }
  for (size_t i = 0; i < batchCount; i++)
  {
    string m_iStr = "";
    for (size_t j = 0; j < msgPerBatch; j++)
    {
      auto idx = i * msgPerBatch + j;
      if (idx >= msgCount)
        break;

      auto mi = ConvertUtils::toBinary(m[idx]);
      ConvertUtils::fixBinary(mi, msgSize);

      string miStr = "";
      for (size_t r = 0; r < slotsPerMsg; r++)
      {
        auto bi = mi[r * slotSize];
        miStr = (bi == 0 ? "0" : "1") + miStr;
      }
      m_iStr = miStr + m_iStr;
    }
    auto m_i = ConvertUtils::binaryStringToZZ(m_iStr);
    auto r = crypto->pickRandom();
    auto c = crypto->encrypt(m_i, r);
    m_.append(m_i);
    Rm_.append(r);
    Cm_.append(c);
  }
}

void CBatchEnc::setCipher(const Vec<ZZ_p> &Cm,
                          const Vec<ZZ_p> &Cm_,
                          const Vec<ZZ_p> &CRj)
{
  if (Cm.length() != msgCount || Cm_.length() != batchCount || CRj.length() != rangeProofCount)
    throw invalid_argument("cipher count do not match with settings");

  this->Cm = Cm;
  this->Cm_ = Cm_;
  this->CRj = CRj;
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
    ConvertUtils::append(seed, ConvertUtils::toBinary(v));
  for (auto v : Cm_)
    ConvertUtils::append(seed, ConvertUtils::toBinary(v));
  for (auto v : CRj)
    ConvertUtils::append(seed, ConvertUtils::toBinary(v));

  SetSeed(seed.data(), seed.size());
  auto Ljir = RandomBits_ZZ(len * 8);
  return ConvertUtils::toBinary(Ljir);
}

Vec<ZZ_p> CBatchEnc::calculateLj(const binary_t &Ljir)
{
  ZZ_pPush push(GP_P);

  Vec<ZZ_p> Lj;

  auto two32 = conv<ZZ>(2) << (slotSize * 8 - 1);
  Vec<ZZ_p> two32s; // [1, 2^32, 2^64, ... , 2^(32 * (r-1))]
  MathUtils::powerVecZZ_p(conv<ZZ_p>(two32), slotsPerMsg, GP_P, two32s);

  // SUM( lj * bri ) + R'j = L'j
  for (size_t j = 0; j < rangeProofCount; j++)
  {
    auto lj = Rj[j]; // R'j

    for (size_t i = 0; i < msgCount; i++)
    {
      auto mi = ConvertUtils::toBinary(m[i]);
      ConvertUtils::fixBinary(mi, msgSize);

      size_t ji = j * msgCount * slotsPerMsg + i * slotsPerMsg;
      for (size_t r = 0; r < slotsPerMsg; r++)
      {
        auto bi = mi[r * slotSize];
        if (bi == 1)
        {
          auto jir = ji + r;
          auto idx1 = jir & 7;  // mod 8
          auto idx2 = jir >> 3; // div 8
          auto ljir = (Ljir[idx2] >> idx1) & 1;
          if (ljir == 1)
            add(lj, lj, 1); // lj * bri
        }
      }
    }
    Lj.append(lj);
  }
  return Lj;
}

void CBatchEnc::wireUp(const binary_t &Ljir, const Vec<ZZ_p> &Lj)
{
  ZZ_pPush push(GP_P);

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
  auto ZERO = ZZ_p();
  auto ONE = conv<ZZ_p>(1);
  auto NEG_ONE = conv<ZZ_p>(-1);

  // bri * (bri - 1) = 0
  size_t briOffset = gateCount;
  for (size_t i = 0; i < msgCount; i++)
  {
    for (size_t r = 0; r < slotsPerMsg; r++)
    {
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

  auto two32 = conv<ZZ>(2) << (slotSize * 8 - 1); // 2^32
  Vec<ZZ_p> two32s;                               // [1, 2^32, 2^64, ... , 2^(32 * (r-1))]
  MathUtils::powerVecZZ_p(conv<ZZ_p>(two32), slotsPerMsg, GP_P, two32s);
  // linear: SUM( 2^(32 * r)) * bri ) = mi
  //         SUM - mi = 0
  for (size_t i = 0; i < msgCount; i++)
  {
    q = addLinear();

    // -mi
    Wqa[q - 1]->cell(0, encMOffset + encCirN * i, NEG_ONE);

    auto bIdx = i * slotsPerMsg;
    auto offset = briOffset + bIdx;
    for (size_t r = 0; r < slotsPerMsg; r++)
    {
      // 2^(32 * r) * bri
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
      for (size_t r = 0; r < slotsPerMsg; r++)
      {
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
  Vec<ZZ_p> TWOs;
  MathUtils::powerVecZZ_p(conv<ZZ_p>(2), slotsPerMsg * msgPerBatch, GP_P, TWOs);
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

void CBatchEnc::run(const binary_t &Ljir, const Vec<ZZ_p> &Lj)
{
  ZZ_pPush push(GP_P);

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
    cir->run(conv<ZZ>(m), r);
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

  auto ONE = conv<ZZ_p>(1);
  auto ZERO = ZZ_p();
  auto NEG_ONE = conv<ZZ_p>(-1);

  // bri * (bri - 1) = 0
  size_t briOffset = offset;
  for (size_t i = 0; i < msgCount; i++)
  {
    auto bIdx = i * slotsPerMsg;
    offset = briOffset + bIdx;
    auto mi = ConvertUtils::toBinary(m[i]);
    ConvertUtils::fixBinary(mi, msgSize);
    for (size_t r = 0; r < slotsPerMsg; r++)
    {
      // gate: bri * (bri - 1) = 0
      auto bi = mi[r * slotSize];
      A->cell(0, offset + r, bi == 0 ? ZERO : ONE);
      B->cell(0, offset + r, bi == 0 ? NEG_ONE : ZERO);
    }
  }
}
