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

shared_ptr<Matrix> Polynomial::get(int i)
{
  auto it = values.find(i);
  if (it == values.end())
  {
    return Matrix::ZERO();
  }

  return it->second;
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

shared_ptr<Polynomial> Polynomial::dot(const shared_ptr<Matrix> &b, const shared_ptr<Integer> modulus)
{
  auto ret = make_shared<Polynomial>();

  for (auto it : values)
  {
    auto d = it.first;
    ret->put(d, it.second->dot(b, modulus));
  }
  return ret;
}

shared_ptr<Polynomial> Polynomial::mul(const shared_ptr<Polynomial> &b, const shared_ptr<Integer> modulus)
{
  auto ret = make_shared<Polynomial>();

  for (int d1 : this->getDegrees())
  {
    auto ai = this->get(d1);
    for (int d2 : b->getDegrees())
    {
      auto bi = b->get(d2);
      auto ab = ai->mul(bi, modulus);
      auto d = d1 + d2;
      ret->put(d, ret->get(d)->add(ab, modulus));
    }
  }
  return ret;
}

// // Scalar multiplication for all matrix coefficients
// // eg. a(x) = a1 * x + a2 * x^3
// // b * a(x) = (b*a1) * x + (b*a2) * x^3
// shared_ptr<Polynomial> Polynomial::mul(const shared_ptr<Integer> &b, const shared_ptr<Integer> modulus)
// {
//   auto ret = this->clone();

//   auto zero = make_shared<Matrix>(1, 1);
//   bool isMod = !Integer::ZERO()->eq(modulus);

//   for (int i = ret->getSmallestDegree(); i <= ret->getLargestDegree(); i++)
//   {
//     auto mi = ret->get(i);
//     if (mi->eq(zero))
//       continue;
//     else
//       ret->put(mi->mul(b, modulus), i);
//   }
//   return ret;
// }

// // Cross product for all matrix coefficients with a vector
// // eg. a(x) = a1 * x + a2 * x^3
// // b * a(x) = (b*a1) * x + (b*a2) * x^3
// // b is a vector
// shared_ptr<Polynomial> Polynomial::mul(const shared_ptr<Matrix> &b, const shared_ptr<Integer> modulus)
// {
//   auto ret = this->clone();

//   auto zero = make_shared<Matrix>(1, 1);

//   for (int i = ret->getSmallestDegree(); i <= ret->getLargestDegree(); i++)
//   {
//     auto mi = ret->get(i);
//     if (mi->eq(zero))
//       continue;
//     else
//       ret->put(mi->mul(b, modulus), i);
//   }
//   return ret;
// }

// // Dot product for all matrix coefficients
// // eg. a(x) = a1 * x + a2 * x^3
// // b . a(x) = (b.a1) * x + (b.a2) * x^3
// shared_ptr<Polynomial> Polynomial::dot(const shared_ptr<Matrix> &b, const shared_ptr<Integer> modulus)
// {
//   auto ret = this->clone();

//   auto zero = make_shared<Matrix>(1, 1);

//   for (int i = ret->getSmallestDegree(); i <= ret->getLargestDegree(); i++)
//   {
//     auto mi = ret->get(i);
//     if (mi->eq(zero))
//       continue;
//     else
//       ret->put(mi->dot(b, modulus), i);
//   }
//   return ret;
// }

// // Multiply two polynomial
// // eg.    a(x) = a1 * x + a2 * x^2
// //        b(x) = b1     + b2 * x
// // a(x) * b(x) = (a1*b1) * x + (a1*b2) * x^2 + (a2*b1) * x^2 + (a2*b2) * x^3
// // a(x) * b(x) = (a1*b1) * x + (a1*b2 + a2*b1) * x^2 + (a2*b2) * x^3
// shared_ptr<Polynomial> Polynomial::mul(const shared_ptr<Polynomial> &b, const shared_ptr<Integer> modulus)
// {
//   auto ret = make_shared<Polynomial>();

//   auto zero = make_shared<Matrix>(1, 1);

//   for (int i = this->getSmallestDegree(); i <= this->getLargestDegree(); i++)
//   {
//     for (int j = b->getSmallestDegree(); j <= b->getLargestDegree(); j++)
//     {
//       if (this->get(i)->eq(zero) || b->get(j)->eq(zero))
//         continue;

//       int degree = i + j;
//       auto origin = ret->get(degree);

//       auto adding = this->get(i)->mul(b->get(j), modulus);

//       // ret[degree] += a[i] * b[i]
//       if (origin)
//         adding = origin->add(adding, modulus);
//       ret->put(adding, degree);
//     }
//   }

//   return ret;
// }

// json Polynomial::toJson()
// {
//   json output = json::array();
//   int index = 0;
//   for (int i = this->getSmallestDegree(); i <= this->getLargestDegree(); i++)
//   {
//     json element = json::object();
//     auto matrix = this->get(i)->toJson();
//     element["degree"] = i;
//     element["matrix"] = matrix;
//     output.push_back(element);
//   }

//   return output;
// }

// string Polynomial::toString()
// {
//   return this->toJson().dump();
// }
