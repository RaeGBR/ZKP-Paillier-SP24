#include "./Polynomial.hpp"
#include <algorithm> // std::min

Polynomial::Polynomial() {}

// D is the least significant degree
Polynomial::Polynomial(const vector<shared_ptr<Matrix>> &ci, int d)
{
  for (size_t i = 0; i < ci.size(); i++)
  {
    if (!ci[i]->eq(Matrix::ZERO()))
    {
      values[d + i] = ci[i];
    }
  }
  lsd = d;
  msd = d + ci.size() - 1;
}

shared_ptr<Polynomial> Polynomial::clone()
{
  auto ret = make_shared<Polynomial>();
  for (auto const &el : values)
  {
    ret->put(el.first, el.second);
  }
  return ret;
}

shared_ptr<Polynomial> Polynomial::t()
{
  auto ret = make_shared<Polynomial>();
  for (auto const &el : values)
  {
    ret->put(el.first, el.second->t());
  }
  return ret;
}

int Polynomial::getSmallestDegree()
{
  return this->lsd;
}

int Polynomial::getLargestDegree()
{
  return this->msd;
}

vector<int> Polynomial::getDegrees()
{
  vector<int> ret;
  for (auto const &el : values)
  {
    ret.push_back(el.first);
  }
  return ret;
}

bool Polynomial::degreeExists(int i)
{
  auto it = values.find(i);
  return it != values.end();
}

shared_ptr<Matrix> Polynomial::get(int i, const shared_ptr<Matrix> &defaultValue)
{
  if (!degreeExists(i))
  {
    return defaultValue != nullptr ? defaultValue : zero();
  }

  return values[i];
}

shared_ptr<Matrix> Polynomial::zero()
{
  if (values.empty())
    return Matrix::ZERO();

  auto d = getSmallestDegree();
  auto el = values[d];
  auto ret = make_shared<Matrix>(el->m, el->n);
  return ret;
}

void Polynomial::put(int i, const shared_ptr<Matrix> &c)
{
  if (values.empty())
  {
    lsd = i;
    msd = i;
  }
  else if (i < lsd)
  {
    lsd = i;
  }
  else if (i > msd)
  {
    msd = i;
  }

  values[i] = c;
}

void Polynomial::erase(int i)
{
  values.erase(i);

  if (i == lsd)
  {
    lsd = values.begin()->first;
  }
  else if (i == msd)
  {
    msd = values.rbegin()->first;
  }
}

shared_ptr<Polynomial> Polynomial::add(const shared_ptr<Polynomial> &b, const shared_ptr<Integer> modulus)
{
  auto ret = this->clone();

  for (int d : b->getDegrees())
  {
    auto ci = ret->get(d)->add(b->get(d), modulus);
    ret->put(d, ci);
  }
  return ret;
}

shared_ptr<Polynomial> Polynomial::mul(const shared_ptr<Integer> &b, const shared_ptr<Integer> modulus)
{
  auto ret = make_shared<Polynomial>();

  for (auto it : values)
  {
    auto d = it.first;
    ret->put(d, it.second->mul(b, modulus));
  }
  return ret;
}

shared_ptr<Polynomial> Polynomial::mul(const shared_ptr<Matrix> &b, const shared_ptr<Integer> modulus)
{
  auto ret = make_shared<Polynomial>();

  for (auto it : values)
  {
    auto d = it.first;
    ret->put(d, it.second->mul(b, modulus));
  }
  return ret;
}

shared_ptr<Polynomial> Polynomial::inner(const shared_ptr<Matrix> &b, const shared_ptr<Integer> modulus)
{
  auto ret = make_shared<Polynomial>();

  for (auto it : values)
  {
    auto d = it.first;
    ret->put(d, it.second->inner(b, modulus));
  }
  return ret;
}

shared_ptr<Polynomial> Polynomial::mul(const shared_ptr<Polynomial> &b, const shared_ptr<Integer> modulus)
{
  auto ret = make_shared<Polynomial>();
  auto aFirst = get(getSmallestDegree());
  auto bFirst = b->get(b->getSmallestDegree());
  auto isVectorPoly = aFirst->m == 1 && bFirst->m == 1 && aFirst->n == bFirst->n;
  auto isMod = !modulus->eq(Integer::ZERO());

  for (auto it1 : values)
  {
    int d1 = it1.first;
    auto ai = it1.second;
    for (auto it2 : b->values)
    {
      int d2 = it2.first;
      auto bi = it2.second;
      shared_ptr<Matrix> ab;
      if (isVectorPoly)
      {
        shared_ptr<Integer> sum = Integer::ZERO();
        for (auto it3 : ai->values[0])
        {
          size_t j = it3.first;
          auto v1 = it3.second;
          if (bi->cellExists(0, j))
          {
            auto v2 = bi->cell(0, j);
            sum = isMod ? sum->add(v1->modMul(v2, modulus))->mod(modulus) : sum->add(v1->mul(v2));
          }
        }
        ab = make_shared<Matrix>(1, 1);
        ab->cell(0, 0, sum);
      }
      else
      {
        ab = ai->mul(bi, modulus);
      }
      auto d = d1 + d2;
      ab = ret->degreeExists(d) ? ret->get(d)->add(ab, modulus) : ab;
      ret->put(d, ab);
    }
  }
  return ret;
}

shared_ptr<Matrix> Polynomial::eval(const shared_ptr<Integer> &x, const shared_ptr<Integer> modulus)
{
  if (values.empty())
    return Matrix::ZERO();

  bool isMod = !modulus->eq(Integer::ZERO());
  auto first = get(getSmallestDegree());
  auto ret = make_shared<Matrix>(first->m, first->n);

  for (auto el : values)
  {
    if (!isMod)
    {
      auto i = make_shared<IntegerImpl>(el.first);
      auto xi = i->eq(Integer::ZERO()) ? Integer::ONE() : x->pow(i);
      auto v = el.second->mul(xi);
      ret = ret->add(v);
    }
    else
    {
      bool isNeg = el.first < 0;
      auto i = make_shared<IntegerImpl>(isNeg ? -el.first : el.first);
      auto xi = i->eq(Integer::ZERO()) ? Integer::ONE() : x->modPow(i, modulus);
      if (isNeg)
      {
        xi = xi->inv(modulus);
      }
      auto v = el.second->mul(xi, modulus);
      ret = ret->add(v, modulus);
    }
  }

  return ret;
}

json Polynomial::toJson()
{
  json output = json::array();
  for (auto el : values)
  {
    json obj = json::object();
    obj["degree"] = el.first;
    obj["matrix"] = el.second->toJson();
    output.push_back(obj);
  }
  return output;
}

string Polynomial::toString()
{
  string ret = "f(X) =";
  if (values.empty())
  {
    ret += " 0";
    return ret;
  }

  auto j = this->toJson();
  for (size_t i = 0; i < j.size(); i++)
  {
    if (i > 0)
    {
      ret += " + ";
    }
    ret += j[i]["matrix"].dump() + " X^" + j[i]["degree"].dump();
  }
  return ret;
}
