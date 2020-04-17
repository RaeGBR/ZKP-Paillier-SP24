#include "./CBase.hpp"

void CBase::copyCircuit(const shared_ptr<CBase> &values, const shared_ptr<CBase> &target)
{
  target->GP_Q = values->GP_Q;
  target->GP_P = values->GP_P;
  target->GP_G = values->GP_G;

  target->Wqa.clear();
  target->Wqb.clear();
  target->Wqc.clear();
  for (auto v : values->Wqa)
    target->Wqa.push_back(v->clone());
  for (auto v : values->Wqb)
    target->Wqb.push_back(v->clone());
  for (auto v : values->Wqc)
    target->Wqc.push_back(v->clone());

  target->Kq = values->Kq;

  target->A = values->A->clone();
  target->B = values->B->clone();
  target->C = values->C->clone();

  target->linearCount = values->linearCount;
  target->gateCount = values->gateCount;
  target->offset = values->offset;
}

CBase::CBase() {}

CBase::CBase(
    const ZZ &GP_Q,
    const ZZ &GP_P,
    const ZZ_p &GP_G)
{
  this->GP_Q = GP_Q;
  this->GP_P = GP_P;
  this->GP_G = GP_G;
  this->A = nullptr;
  this->B = nullptr;
  this->C = nullptr;
  ;
}

CBase::CBase(
    const ZZ &GP_Q,
    const ZZ &GP_P,
    const ZZ_p &GP_G,
    const vector<shared_ptr<Matrix>> &Wqa,
    const vector<shared_ptr<Matrix>> &Wqb,
    const vector<shared_ptr<Matrix>> &Wqc,
    const Vec<ZZ_p> &Kq,
    const shared_ptr<Matrix> &A,
    const shared_ptr<Matrix> &B,
    const shared_ptr<Matrix> &C)
{
  this->GP_Q = GP_Q;
  this->GP_P = GP_P;
  this->GP_G = GP_G;
  this->Wqa = Wqa;
  this->Wqb = Wqb;
  this->Wqc = Wqc;
  this->Kq = Kq;
  this->A = A;
  this->B = B;
  this->C = C;

  linearCount = Wqa.size();

  if (A != nullptr)
    gateCount = A->m * A->n;
}

void CBase::shift(size_t n)
{
  for (auto v : Wqa)
    v->shift(n);
  for (auto v : Wqb)
    v->shift(n);
  for (auto v : Wqc)
    v->shift(n);

  offset += n;
}

void CBase::group(size_t n, size_t m)
{
  for (size_t i = 0; i < linearCount; i++)
  {
    Wqa[i] = Wqa[i]->group(n);
    Wqb[i] = Wqb[i]->group(n);
    Wqc[i] = Wqc[i]->group(n);
  }

  if (A != nullptr)
    A = A->group(n, m);
  if (B != nullptr)
    B = B->group(n, m);
  if (C != nullptr)
    C = C->group(n, m);
}

void CBase::trim()
{
  for (size_t i = 0; i < linearCount; i++)
  {
    Wqa[i]->trim();
    Wqb[i]->trim();
    Wqc[i]->trim();
  }
}

void CBase::append(const shared_ptr<CBase> &b)
{
  if (b->gateCount == 0 || b->linearCount == 0)
    throw invalid_argument("appended circuit is empty");

  auto oldN = gateCount;
  auto oldQ = linearCount;
  if (gateCount == 0)
  {
    A = b->A->clone();
    B = b->B->clone();
    C = b->C->clone();
  }
  else
  {
    this->A->extend(b->gateCount);
    this->B->extend(b->gateCount);
    this->C->extend(b->gateCount);
    for (size_t i = 0; i < b->gateCount; i++)
    {
      this->A->cell(0, oldN + i, b->A->cell(0, i));
      this->B->cell(0, oldN + i, b->B->cell(0, i));
      this->C->cell(0, oldN + i, b->C->cell(0, i));
    }
  }
  gateCount += b->gateCount;

  for (size_t i = 0; i < b->linearCount; i++)
  {
    auto wqa = b->Wqa[i]->clone();
    auto wqb = b->Wqb[i]->clone();
    auto wqc = b->Wqc[i]->clone();
    wqa->shift(oldN);
    wqb->shift(oldN);
    wqc->shift(oldN);
    this->Wqa.push_back(wqa);
    this->Wqb.push_back(wqb);
    this->Wqc.push_back(wqc);
    this->Kq.append(b->Kq[i]);
  }
  linearCount += b->linearCount;
}

size_t CBase::assignValues(const shared_ptr<CBase> &b, size_t offset)
{
  if (A == nullptr)
    A = make_shared<Matrix>(1, gateCount);
  if (B == nullptr)
    B = make_shared<Matrix>(1, gateCount);
  if (C == nullptr)
    C = make_shared<Matrix>(1, gateCount);

  size_t ret = offset + b->gateCount;
  if (ret > gateCount)
    throw invalid_argument("cannot assign values to circuit, exceed the max gate bound");

  for (auto it : b->A->values[0])
  {
    size_t i = it.first;
    auto v = it.second;
    A->cell(0, offset + i, v);
  }
  for (auto it : b->B->values[0])
  {
    size_t i = it.first;
    auto v = it.second;
    B->cell(0, offset + i, v);
  }
  for (auto it : b->C->values[0])
  {
    size_t i = it.first;
    auto v = it.second;
    C->cell(0, offset + i, v);
  }

  return ret;
}

size_t CBase::addGate(size_t n)
{
  if (gateCount == 0)
  {
    A = make_shared<Matrix>(1, n);
    B = make_shared<Matrix>(1, n);
    C = make_shared<Matrix>(1, n);
  }
  else
  {
    A->extend(n);
    B->extend(n);
    C->extend(n);
  }
  gateCount += n;
  return gateCount;
}

size_t CBase::addLinear()
{
  Wqa.push_back(make_shared<Matrix>(1, gateCount));
  Wqb.push_back(make_shared<Matrix>(1, gateCount));
  Wqc.push_back(make_shared<Matrix>(1, gateCount));
  Kq.append(ZZ_p());
  return ++linearCount;
}

shared_ptr<CircuitZKPVerifier> CBase::generateVerifier(const Vec<ZZ_p> &gi)
{
  auto mnCfg = CircuitZKPVerifier::calcMN(gateCount);
  auto m = mnCfg[0];
  auto n = mnCfg[1];

  auto zkp = make_shared<CircuitZKPVerifier>(
      GP_Q, GP_P, GP_G,
      vector<shared_ptr<Matrix>>({}),
      vector<shared_ptr<Matrix>>({}),
      vector<shared_ptr<Matrix>>({}),
      Kq,
      m, n, linearCount);

  zkp->commitScheme->gi = gi;

  convertWire(this->Wqa, zkp->Wqa, m, n);
  convertWire(this->Wqb, zkp->Wqb, m, n);
  convertWire(this->Wqc, zkp->Wqc, m, n);

  return zkp;
}

void CBase::convertWire(const vector<shared_ptr<Matrix>> &source,
                        map<size_t, map<size_t, map<size_t, ZZ_p>>> &target,
                        size_t m, size_t n)
{
  auto N = n * m;
  for (size_t q = 0; q < source.size(); q++)
  {
    if (source[q]->m != 1)
      throw invalid_argument("wire already converted");

    if (source[q]->n > N)
      throw invalid_argument("wire convert failed, N exceed the matrix dimension");

    for (auto it : source[q]->values[0])
    {
      size_t i = it.first;
      auto v = it.second;
      size_t x = i / n;
      size_t y = i % n;

      if (IsZero(v))
        continue;

      if (target[x].find(q) == target[x].end())
      {
        target[x][q] = map<size_t, ZZ_p>();
      }
      target[x][q][y] = v;
    }
  }
}

shared_ptr<CircuitZKPProver> CBase::generateProver(const Vec<ZZ_p> &gi)
{
  auto zkp = generateVerifier(gi);
  auto prover = make_shared<CircuitZKPProver>(zkp,
                                              A->group(zkp->n, zkp->m),
                                              B->group(zkp->n, zkp->m),
                                              C->group(zkp->n, zkp->m));
  return prover;
}

json CBase::toJson()
{
  json output = json::object();
  output["Wqa"] = json::array();
  output["Wqb"] = json::array();
  output["Wqc"] = json::array();
  output["Kq"] = json::array();
  output["A"] = A != nullptr ? A->toJson() : json::array();
  output["B"] = B != nullptr ? B->toJson() : json::array();
  output["C"] = C != nullptr ? C->toJson() : json::array();

  for (size_t i = 0; i < linearCount; i++)
  {
    output["Wqa"].push_back(Wqa[i]->toJson());
    output["Wqb"].push_back(Wqb[i]->toJson());
    output["Wqc"].push_back(Wqc[i]->toJson());
    output["Kq"].push_back(ConvertUtils::toString(Kq[i]));
  }
  return output;
}

string CBase::toString()
{
  /*
  return this->toJson().dump();
  /*/
  auto j = this->toJson();
  string ret = "";
  ret += "A:\n" + j["A"].dump() + "\n";
  ret += "B:\n" + j["B"].dump() + "\n";
  ret += "C:\n" + j["C"].dump() + "\n";
  for (size_t i = 0; i < linearCount; i++)
  {
    ret += "W[" + to_string(i) + "]:\n";
    ret += j["Wqa"][i].dump() + "\n";
    ret += j["Wqb"][i].dump() + "\n";
    ret += j["Wqc"][i].dump() + "\n";
    ret += ((string)j["Kq"][i]) + "\n";
  }
  return ret;
  //*/
}
