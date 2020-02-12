#include "./CBase.hpp"

using namespace polyu;

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

  target->Kq.clear();
  target->Kq.insert(target->Kq.begin(), values->Kq.begin(), values->Kq.end());

  target->A = values->A->clone();
  target->B = values->B->clone();
  target->C = values->C->clone();

  target->linearCount = values->linearCount;
  target->gateCount = values->gateCount;
  target->offset = values->offset;
}

CBase::CBase() {}

CBase::CBase(
    const shared_ptr<Integer> &GP_Q,
    const shared_ptr<Integer> &GP_P,
    const shared_ptr<Integer> &GP_G,
    const vector<shared_ptr<Matrix>> &Wqa,
    const vector<shared_ptr<Matrix>> &Wqb,
    const vector<shared_ptr<Matrix>> &Wqc,
    const vector<shared_ptr<Integer>> &Kq,
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
      this->A->values[0][oldN + i] = b->A->values[0][i];
      this->B->values[0][oldN + i] = b->B->values[0][i];
      this->C->values[0][oldN + i] = b->C->values[0][i];
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
    this->Kq.push_back(b->Kq[i]);
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

  for (size_t i = 0; i < b->gateCount; i++)
  {
    A->values[0][offset + i] = b->A->values[0][i];
    B->values[0][offset + i] = b->B->values[0][i];
    C->values[0][offset + i] = b->C->values[0][i];
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
  Kq.push_back(Integer::ZERO());
  return ++linearCount;
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
    output["Kq"].push_back(Kq[i]->toString());
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
