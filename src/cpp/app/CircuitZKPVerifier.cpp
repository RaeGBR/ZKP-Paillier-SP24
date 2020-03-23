#include "./CircuitZKPVerifier.hpp"

using namespace polyu;

vector<size_t> CircuitZKPVerifier::calcMN(size_t _N)
{
  double N = _N;
  double n = ceil(sqrt(N));
  double m = ceil(N / n);
  vector<size_t> ret({(size_t)m, (size_t)n});
  return ret;
}

vector<size_t> CircuitZKPVerifier::calcM1M2N(size_t m)
{
  double N = 3 * m + 4 * m + 2;
  double n = ceil(sqrt(N));
  double m1 = ceil((3 * m) / n);
  double m2 = ceil((4 * m + 2) / n);
  vector<size_t> ret({(size_t)m1, (size_t)m2, (size_t)n});
  return ret;
}

CircuitZKPVerifier::CircuitZKPVerifier(
    const ZZ &GP_Q,
    const ZZ &GP_P,
    const ZZ_p &GP_G,
    const vector<shared_ptr<Matrix>> &Wqa,
    const vector<shared_ptr<Matrix>> &Wqb,
    const vector<shared_ptr<Matrix>> &Wqc,
    const Vec<ZZ_p> &Kq)
    : CircuitZKPVerifier(GP_Q, GP_P, GP_G, Wqa, Wqb, Wqc, Kq, 0, 0) {}

CircuitZKPVerifier::CircuitZKPVerifier(
    const ZZ &GP_Q,
    const ZZ &GP_P,
    const ZZ_p &GP_G,
    const vector<shared_ptr<Matrix>> &Wqa,
    const vector<shared_ptr<Matrix>> &Wqb,
    const vector<shared_ptr<Matrix>> &Wqc,
    const Vec<ZZ_p> &Kq,
    size_t m,
    size_t n)
{
  this->GP_Q = GP_Q;
  this->GP_P = GP_P;
  this->GP_G = GP_G;
  this->Wqa = Wqa;
  this->Wqb = Wqb;
  this->Wqc = Wqc;
  this->Kq = Kq;

  this->Q = Wqa.size();
  if (this->Q <= 0 || this->Q != Wqb.size() || this->Q != Wqc.size())
    throw invalid_argument("the number of linear constrians are different in Wqa, Wqb and Wqc");

  m = m > 0 ? m : Wqa[0]->m;
  n = n > 0 ? n : Wqa[0]->n;
  this->m = m;
  this->n = n;
  this->N = m * n;
  this->M = N + m;

  auto txCfg = CircuitZKPVerifier::calcM1M2N(m);
  this->txM1 = txCfg[0];
  this->txM2 = txCfg[1];
  this->txN = txCfg[2];

  this->commitScheme = make_shared<PolynomialCommitment>(this->GP_Q, this->GP_P, this->GP_G, max(this->txN, this->n));
}

Vec<ZZ_p> &CircuitZKPVerifier::getY_Mq(const ZZ_p &y)
{
  if (!IsZero(cachedY_Mq))
    return cachedY_Mq;

  ZZ_pPush push(GP_P);
  cachedY_Mq.SetLength(Q);

  ZZ_p y_M = power(y, this->M); // y^M
  mul(cachedY_Mq[0], y_M, y);   // y^(M+1)
  for (size_t q = 1; q < Q; q++)
  {
    mul(cachedY_Mq[q], cachedY_Mq[q - 1], y); // y^(M+q)
  }
  return cachedY_Mq;
}

ZZ_p CircuitZKPVerifier::getY_Mq(const ZZ_p &y, size_t q)
{
  return getY_Mq(y)[q - 1];
}

Vec<ZZ_p> &CircuitZKPVerifier::getY(const ZZ_p &y)
{
  if (!IsZero(cachedY))
    return cachedY;

  MathUtils::powerVecZZ_p(y, m + 1, GP_P, cachedY); // [1, y, y^2, ... , y^m]
  return cachedY;
}

Vec<ZZ_p> &CircuitZKPVerifier::getY_(const ZZ_p &y)
{
  if (!IsZero(cachedY_))
    return cachedY_;

  ZZ_pPush push(GP_P);
  cachedY_.SetLength(n);

  power(cachedY_[0], y, m); // y^m
  for (size_t i = 1; i < n; i++)
  {
    // y^m * previous object (so [y^m, y^2m, ... , y^mn])
    mul(cachedY_[i], cachedY_[i - 1], cachedY_[0]);
  }
  return cachedY_;
}

void CircuitZKPVerifier::setY(const ZZ_p &y)
{
  this->cachedY_Mq.SetLength(0);
  this->cachedY.SetLength(0);
  this->cachedY_.SetLength(0);
  getY_Mq(y);
  getY(y);
  getY_(y);
}

shared_ptr<Matrix> CircuitZKPVerifier::Wai(size_t i, const ZZ_p &y)
{
  if (i <= 0 || i > m)
    throw invalid_argument("i should between 1 to m");

  ZZ_p tmp;

  auto ret = make_shared<Matrix>(1, n);
  for (size_t q = 1; q <= Q; q++)
  {
    // zero row may trimmed, only process on non-zero rows
    if (Wqa[q - 1]->rowExists(i - 1))
    {
      auto yMq = getY_Mq(y, q);
      for (auto it : Wqa[q - 1]->values[i - 1])
      {
        size_t j = it.first;
        auto v = it.second;

        if (IsOne(v))
        {
          tmp = yMq;
        }
        else
        {
          mul(tmp, v, yMq);
        }
        if (ret->cellExists(0, j))
        {
          add(tmp, tmp, ret->cell(0, j));
        }
        ret->cell(0, j, tmp);
      }
    }
  }
  return ret;
}

shared_ptr<Matrix> CircuitZKPVerifier::Wbi(size_t i, const ZZ_p &y)
{
  if (i <= 0 || i > m)
    throw invalid_argument("i should between 1 to m");

  ZZ_p tmp;

  auto ret = make_shared<Matrix>(1, n);
  for (size_t q = 1; q <= Q; q++)
  {
    // zero row may trimmed, only process on non-zero rows
    if (Wqb[q - 1]->rowExists(i - 1))
    {
      auto yMq = getY_Mq(y, q);
      for (auto it : Wqb[q - 1]->values[i - 1])
      {
        size_t j = it.first;
        auto v = it.second;

        if (IsOne(v))
        {
          tmp = yMq;
        }
        else
        {
          mul(tmp, v, yMq);
        }
        if (ret->cellExists(0, j))
        {
          add(tmp, tmp, ret->cell(0, j));
        }
        ret->cell(0, j, tmp);
      }
    }
  }
  return ret;
}

shared_ptr<Matrix> CircuitZKPVerifier::Wci(size_t i, const ZZ_p &y)
{
  if (i <= 0 || i > m)
    throw invalid_argument("i should between 1 to m");

  ZZ_p tmp;

  auto ret = make_shared<Matrix>(1, n);
  for (size_t q = 1; q <= Q; q++)
  {
    // zero row may trimmed, only process on non-zero rows
    if (Wqc[q - 1]->rowExists(i - 1))
    {
      auto yMq = getY_Mq(y, q);
      for (auto it : Wqc[q - 1]->values[i - 1])
      {
        size_t j = it.first;
        auto v = it.second;

        if (IsOne(v))
        {
          tmp = yMq;
        }
        else
        {
          mul(tmp, v, yMq);
        }
        if (ret->cellExists(0, j))
        {
          add(tmp, tmp, ret->cell(0, j));
        }
        ret->cell(0, j, tmp);
      }
    }
  }

  auto Y_ = getY_(y);
  auto yi = getY(y)[i];
  for (size_t j = 0; j < n; j++)
  {
    mul(tmp, Y_[j], yi);
    sub(tmp, ret->cell(0, j), tmp);
    ret->cell(0, j, tmp);
  }

  return ret;
}

ZZ_p CircuitZKPVerifier::K(const ZZ_p &y)
{
  ZZ_p ret;
  for (size_t q = 1; q <= Q; q++)
  {
    auto kq = Kq(q);
    if (IsZero(kq))
      continue;

    ZZ_p KqYmq;

    if (IsOne(kq))
    {
      KqYmq = getY_Mq(y, q);
    }
    else
    {
      mul(KqYmq, kq, getY_Mq(y, q));
    }
    add(ret, ret, KqYmq);
  }
  return ret;
}

void CircuitZKPVerifier::createSx(const ZZ_p &y, vector<ZZ_pX> &sx)
{
  ZZ_pPush push(GP_P);
  sx.clear();
  for (size_t i = 0; i < n; i++)
  {
    sx.push_back(ZZ_pX());
  }

  // s(X) = SUM(Wai(y) * y^-i * X^-i) + SUM(Wbi(y) * X^i) + X^-m * SUM(Wci(y) * X^-i)
  //      = (X^2m) * ( SUM(Wai(y) * y^-i * X^(2m-i)) + SUM(Wbi(y) * X^(2m+i)) + SUM(Wci(y) * X^(m-i)) )
  Vec<ZZ_p> Y = getY(y); // [1, y, y^2, ... , y^m]
  ZZ_p tmp;

  Timer::start("sx.wai");
  for (size_t i = 1; i <= m; i++)
  {
    auto wai = Wai(i, y);
    auto yi = inv(Y[i]);

    for (auto it : wai->values[0])
    {
      auto j = it.first;
      auto v = it.second;
      mul(tmp, v, yi);
      SetCoeff(sx[j], 2 * m - i, tmp);
    }
  }
  Timer::end("sx.wai");

  Timer::start("sx.wbi");
  for (size_t i = 1; i <= m; i++)
  {
    auto wbi = Wbi(i, y);
    for (auto it : wbi->values[0])
    {
      auto j = it.first;
      auto v = it.second;
      SetCoeff(sx[j], 2 * m + i, v);
    }
  }
  Timer::end("sx.wbi");

  Timer::start("sx.wci");
  for (size_t i = 1; i <= m; i++)
  {
    auto wci = Wci(i, y);
    for (auto it : wci->values[0])
    {
      auto j = it.first;
      auto v = it.second;
      SetCoeff(sx[j], m - i, v);
    }
  }
  Timer::end("sx.wci");
}

void CircuitZKPVerifier::setCommits(const Vec<ZZ_p> &commits)
{
  if (commits.length() != 3 * m + 1)
    throw invalid_argument("commitments count is not correct");

  ConvertUtils::subVec(commits, commitA, 0, m);
  ConvertUtils::subVec(commits, commitB, m, 2 * m);
  ConvertUtils::subVec(commits, commitC, 2 * m, 3 * m);

  commitD = commits[3 * m];
}

ZZ_p CircuitZKPVerifier::calculateY()
{
  auto seed = ConvertUtils::toBinary(commitD);
  for (size_t i = 0; i < m; i++)
  {
    ConvertUtils::append(seed, ConvertUtils::toBinary(commitA[i]));
    ConvertUtils::append(seed, ConvertUtils::toBinary(commitB[i]));
    ConvertUtils::append(seed, ConvertUtils::toBinary(commitC[i]));
  }

  SetSeed(seed.data(), seed.size());
  auto ret = MathUtils::randZZ_p(GP_P, true);
  return ret;
}

void CircuitZKPVerifier::setPolyCommits(const Vec<ZZ_p> &pc)
{
  if (pc.length() != txM1 + txM2 + 1)
    throw invalid_argument("polynomial commitments count is not correct");

  this->pc = pc;
}

ZZ_p CircuitZKPVerifier::calculateX()
{
  auto seed = ConvertUtils::toBinary(pc[0]);
  for (size_t i = 1; i < pc.length(); i++)
  {
    ConvertUtils::append(seed, ConvertUtils::toBinary(pc[i]));
  }

  SetSeed(seed.data(), seed.size());
  auto ret = MathUtils::randZZ_p(GP_P, true);
  return ret;
}

bool CircuitZKPVerifier::verify(const Vec<ZZ_p> &proofs, const ZZ_p &y, const ZZ_p &x)
{
  Timer::start("verifier.verify");
  // check proof size
  if (proofs.length() != txN + 1 + n + 1)
    return false;

  ZZ_pPush push(GP_Q);

  // extract pe, r, rr from proofs
  Vec<ZZ_p> pe;
  Vec<ZZ_p> r;
  ConvertUtils::subVec(proofs, pe, 0, txN + 1);
  ConvertUtils::subVec(proofs, r, txN + 1, txN + 1 + n);
  ZZ_p rr = proofs[proofs.length() - 1];

  ZZ_p tmp;

  // check PolyVerify
  Timer::start("verifier.polyVerify");
  if (!commitScheme->verify(txM1, txM2, txN, pc, pe, x))
    return false;
  Timer::end("verifier.polyVerify");

  // setting: u = 0
  // check PolyEval = r dot r' - 2K
  Timer::start("verifier.calcV1");
  auto v1 = commitScheme->calcV(txN, pe, x);
  Timer::end("verifier.calcV1");

  Timer::start("verifier.calcV2");
  Timer::start("verifier.setY");
  setY(y); // recalculate cachedY_ and cachedY_Mq
  Timer::end("verifier.setY");

  Timer::start("verifier.sx");
  ZZ_p::init(GP_P);

  // create polynomial s(X)
  vector<ZZ_pX> sx;
  createSx(y, sx);

  // put x into s(X)
  Vec<ZZ_p> sx2;
  sx2.SetLength(n);
  for (size_t i = 0; i < n; i++)
  {
    eval(sx2[i], sx[i], x);
  }
  // shift x^-2m, since the NTL polynomial only support positive degree
  power(tmp, x, -2 * m);
  mul(sx2, sx2, tmp);
  mul(sx2, sx2, 2); // 2 * s(x)
  Timer::end("verifier.sx");

  // r' = r inner y' + 2s(x)
  Timer::start("verifier.r_");
  Vec<ZZ_p> r_;
  r_.SetLength(n);
  const auto y_ = getY_(y);
  for (size_t i = 0; i < n; i++)
  {
    mul(r_[i], r[i], y_[i]);
  }
  add(r_, r_, sx2);
  Timer::end("verifier.r_");

  // r * r' - 2K
  Timer::start("verifier.rr_");
  ZZ_p v2;
  InnerProduct(v2, r, r_); // r * r'
  tmp = K(y);              // put y into K(y)
  mul(tmp, tmp, 2);        // 2 * K(y)
  sub(v2, v2, tmp);        // r * r' - 2K
  Timer::end("verifier.rr_");
  Timer::end("verifier.calcV2");

  if (v1 != v2)
    return false;

  // check commit(r, rr) is correct
  Timer::start("verifier.commitR1");
  auto c1 = commitScheme->commit(r, rr);
  Timer::end("verifier.commitR1");

  Timer::start("verifier.commitR2");
  ZZ_p::init(GP_P);
  ZZ_p c2 = conv<ZZ_p>(1);
  ZZ_p xi = conv<ZZ_p>(1);
  ZZ_p yi = conv<ZZ_p>(1);
  ZZ_p xmi = power(x, m);
  ZZ_p xi_;

  for (int i = 1; i <= m; i++)
  {
    ZZ_p::init(GP_P);
    mul(xi, xi, x);
    mul(yi, yi, y);
    mul(xmi, xmi, x);
    inv(xi_, xi);

    mul(tmp, xi, yi);
    ZZ_p::init(GP_Q);
    power(tmp, commitA[i - 1], conv<ZZ>(tmp));
    mul(c2, c2, tmp);
    power(tmp, commitB[i - 1], conv<ZZ>(xi_));
    mul(c2, c2, tmp);
    power(tmp, commitC[i - 1], conv<ZZ>(xmi));
    mul(c2, c2, tmp);
  }

  ZZ_p::init(GP_P);
  power(tmp, x, 2 * m + 1);
  ZZ_p::init(GP_Q);
  power(tmp, commitD, conv<ZZ>(tmp));
  mul(c2, c2, tmp);
  Timer::end("verifier.commitR2");

  Timer::end("verifier.verify");

  if (c1 != c2)
    return false;

  return true;
}
