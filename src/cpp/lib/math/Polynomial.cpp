#include "./Polynomial.hpp"
#include <algorithm>    // std::min

Polynomial::Polynomial()
{
  this->lsd = 0; 
}

// D is the least significant degree
Polynomial::Polynomial(const vector<shared_ptr<Matrix>> ci, int d)
{
  this->lsd = d;
  values = ci;
}

shared_ptr<Polynomial> Polynomial::clone()
{
  return make_shared<Polynomial>(values, lsd);
}

int Polynomial::getSmallestDegree() {
  return this->lsd;
}

/**
  Poly 3 4 5 6 7 8 9
  Vec. 6 5 4 3 2 1 0
  s = 6; d = 3;
  s + d = 9
*/
int Polynomial::getLargestDegree() {
  return this->lsd + static_cast<int>(values.size());
}

shared_ptr<Matrix> Polynomial::get(int i)
{
  int size = static_cast<int>(values.size());
  if (i > size || i < this->lsd) return nullptr;

  return values[i];
}

void Polynomial::put(const shared_ptr<Matrix> c, int i)
{
  int size = static_cast<int>(values.size());
  // within range -> set vector
  if (i >= getSmallestDegree() && i <= getLargestDegree()) {
    /*
      Poly 3 4 5 6 7 8 9
      Vec. 6 5 4 3 2 1 0
      s = 6; d = 3;
      i = 5 -> v = 4 -> s - (i - d) = 6 - (5 - 3) = 4
      i = 8 -> v = 1 -> s - (i - d) = 6 - (8 - 3) = 1
    */
    int index = size + this->lsd - i;
    values[index] = c;
  } else if (i < getSmallestDegree()) { // smaller than smallest -> push more
    /*
      Poly 3 4 5 6 7 8 9
      Vec. 6 5 4 3 2 1 0
      s = 6; d = 3;
      i = 2 -> v = s + d - i -> v = 7 -> push c to 7
      i = -2 -> v = 6 + 3 + 2 = 11 -> push c to 11 & 0 between i to d
    */
    int index = size + this->lsd - i;
    // Push Zero between the smallest 
    for(int i = size ; i < index ; i++) {
      values.push_back(Matrix(1, 1).t());
    }
    values.push_back(c);
    // set least significant degree
    this->lsd = i;
  } else { // larger than largest -> push at front
    /*
      Poly 3 4 5 6 7 8 9
      Vec. 6 5 4 3 2 1 0
      s = 6; d = 3;
      i = 10 -> push = i - (d + s) = 1
      i = 12 -> push = 12 - 3 + 6 = 3
    */
    int adds = i - this->lsd + size;
    // Push Zero between the largest
    for (int i = 0 ; i < adds ; i++) {
      values.insert(values.begin(), Matrix(1, 1).t());
    }
    values.insert(values.begin(), c);
  }
}

// Erase will just put a zero matrix to the degree
bool Polynomial::erase(int i)
{
  // Out of Bound Erase
  if (i < getSmallestDegree() || i > getLargestDegree()) {
    return false;
  }

  values[i] = Matrix(1, 1).t();
  return true;
}

// eg.    a(x) = a1 * x + a2 * x^3
//        b(x) = b1 * x + b2 * x^2
// a(x) + b(x) = (a1+b1) * x + b2 * x^2 + a2 * x^3
shared_ptr<Polynomial> Polynomial::add(const shared_ptr<Polynomial> &b, const shared_ptr<Integer>  modulus)
{
  // Get the LSD from 2 polynomials
  int smallest = min(this->getSmallestDegree(), b->getSmallestDegree());

  // Get the MSD from 2 polynomials
  int largest  = max(this->getLargestDegree(), b->getLargestDegree());

  // we initial a polynomial first
  auto ret = make_shared<Polynomial>();

  ret->lsd = smallest;

  bool isMod = !Integer::ZERO()->eq(modulus);

  //  a = [a1 00 a2 a3]  = a1*x^3 + a2*x + a3
  //  b =    [b1 00 b2]  = b1*x^2 + b2
  //    = a1*x^3 + b1*x^2 + a2*x + (a3+b3)
  //    = [a1 b1 a2 (a3+b2)]
  for (int i = largest ; i >= smallest ; i--) {
    // need to check is ai & bi are within  modulus ?
    shared_ptr<Matrix> ai = isMod ? this->get(i)->add(0, modulus) : this->get(i);
    shared_ptr<Matrix> bi = isMod ? b->get(i)->add(0, modulus) : b->get(i);
    
    // bi should have the degree if ai is null
    if (!ai) ret->put(bi, i);
    // ai should have the degree if bi is null
    else if (!bi) ret->put(ai, i);
    // add ai & bi
    else ret->put(ai->add(bi,  modulus), i);
  }

  return ret;
}

// Scalar multiplication for all matrix coefficients
// eg. a(x) = a1 * x + a2 * x^3
// b * a(x) = (b*a1) * x + (b*a2) * x^3
shared_ptr<Polynomial> Polynomial::mul(const shared_ptr<Integer> &b, const shared_ptr<Integer> modulus)
{
  auto ret = this->clone();
  for (int i = ret->getSmallestDegree() ; i <= ret->getLargestDegree() ; i++) {
    ret->put(ret->get(i)->mul(b,  modulus), i);
  }
  return ret;
}

// Cross product for all matrix coefficients with a vector
// eg. a(x) = a1 * x + a2 * x^3
// b * a(x) = (b*a1) * x + (b*a2) * x^3
// b is a vector
shared_ptr<Polynomial> Polynomial::mul(const shared_ptr<Matrix> &b, const shared_ptr<Integer> modulus)
{
  auto ret = this->clone();
  for (int i = ret->getSmallestDegree() ; i <= ret->getLargestDegree() ; i++) {
    ret->put(ret->get(i)->mul(b, modulus), i);
  }
  return ret;
}

// Dot product for all matrix coefficients
// eg. a(x) = a1 * x + a2 * x^3
// b . a(x) = (b.a1) * x + (b.a2) * x^3
shared_ptr<Polynomial> Polynomial::dot(const shared_ptr<Matrix> &b, const shared_ptr<Integer> modulus)
{
  auto ret = this->clone();
  for (int i = ret->getSmallestDegree() ; i <= ret->getLargestDegree() ; i++) {
    ret->put(ret->get(i)->dot(b, modulus), i);
  }
  return ret;
}

// Multiply two polynomial
// eg.    a(x) = a1 * x + a2 * x^2
//        b(x) = b1     + b2 * x
// a(x) * b(x) = (a1*b1) * x + (a1*b2) * x^2 + (a2*b1) * x^2 + (a2*b2) * x^3
// a(x) * b(x) = (a1*b1) * x + (a1*b2 + a2*b1) * x^2 + (a2*b2) * x^3
shared_ptr<Polynomial> Polynomial::mul(const shared_ptr<Polynomial> &b, const shared_ptr<Integer> modulus)
{
 auto ret = make_shared<Polynomial>();

 for (int i = this->getSmallestDegree() ; i <= this->getLargestDegree() ; i++) {
   for (int j = b->getSmallestDegree() ; j <= b->getLargestDegree() ; j++) {
     int degree = i + j;
     auto origin = ret->get(degree);
     auto adding = this->get(i)->mul(b->get(j), modulus);

     // ret[degree] += a[i] * b[i]
     if (!origin) adding = origin->add(adding, modulus);
   }
 }

 return ret;
}

json Polynomial::toJson()
{
  json output = json::array();
  int index = 0;
  for (int i = this->getSmallestDegree() ; i <= this->getLargestDegree() ; i++) {
    auto output = this->get(i)->toJson();
    output[index]["degree"] = i;
    output[index]["matrix"] = output;
  }

  return output;
}

string Polynomial::toString()
{
  return this->toJson().dump();
}