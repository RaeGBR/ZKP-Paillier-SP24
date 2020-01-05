#include "./Polynomial.hpp"
#include <algorithm>    // std::min

Polynomial::Polynomial()
{
  this->lsd = 0; 
  values.push_back(make_shared<Matrix>(1, 1)->t());
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
  if (this->values.size() > 0) {
    return this->lsd + this->values.size() - 1;
  } else {
    return this->lsd;
  }
}

shared_ptr<Matrix> Polynomial::get(int i)
{
  int size = values.size();
  if (i > this->getLargestDegree() || i < this->getSmallestDegree()) return nullptr;

  //-1 0 1 2 3
  // 4 3 2 1 0
  // get 0 : (5-1) + -1 - 0 = 3
  // get 1 : (5-1) + -1 - 1 = 2
  // get -1 : (5-1) + -1 - -1 = 4

  // 3 4 5 6 7 8
  // 5 4 3 2 1 0
  // get 3: (6-1) + 3 - 3 = 5
  // get 8: (6-1) + 3 - 8 = 0
  int index = (size - 1) + this->lsd - i;
  return values[index];
}

void Polynomial::clean()
{
  auto zero = make_shared<Matrix>(1, 1)->t();

  // Delete all Zeros until non-zero Coefficient found from Smallest Degree
  bool foundSmallest = false;
  int largest = getLargestDegree();
  for (int i = getSmallestDegree() ; i <= largest && !foundSmallest ; i++) {
    if (!get(i)->eq(zero)) {
      foundSmallest = true;
    } else {
      values.pop_back();
      this->lsd = this->lsd + 1;
    }
  }

  // Delete all Zeros until non-zero Coefficient found from Largest Degree
  bool foundLargest = false;
  for (int i = getLargestDegree() ; i >= getSmallestDegree() && !foundLargest ; i--) {
    if (!get(i)->eq(zero)) { foundLargest = true; }
    else { values.erase(values.begin()); }
  }

  if (foundSmallest || foundLargest) return;

  // All Coefficients are zero -> initial values[0] as zero
  values.clear();
  values.push_back(make_shared<Matrix>(1, 1)->t());
  this->lsd = 0;
}

void Polynomial::put(const shared_ptr<Matrix> c, int i)
{
  int size = values.size();
  // within range -> set vector
  if (i >= getSmallestDegree() && i <= getLargestDegree()) {
    /*
      Poly 3 4 5 6 7 8 9
      Vec. 6 5 4 3 2 1 0
      s = 7; d = 3;
      i = 5 -> v = 4 -> s - (i - d) = 6 - (5 - 3) = 4
      i = 8 -> v = 1 -> s - (i - d) = 6 - (8 - 3) = 1
    */
    int index = (size - 1) + this->lsd - i;
    values[index] = c;
  } else if (i < getSmallestDegree()) { // smaller than smallest -> push more
    /*
      Poly 3 4 5 6 7 8 9
      Vec. 6 5 4 3 2 1 0
      s = 7; d = 3;
      i = 2 -> v = s + d - i -> v = 7 -> push c to 7
      i = -2 -> v = 6 + 3 + 2 = 11 -> push c to 11 & 0 between i to d
    */
    int index = (size - 1) + this->lsd - i;
    // Push Zero between the smallest 
    for(int i = size ; i < index ; i++) {
      values.push_back(Matrix(1, 1).t());
    }
    values.push_back(c);
    // set least significant degree
    this->lsd = i;

    // Clean all Zeros from Smallest & Largest
    clean();
  } else { // larger than largest -> push at front
    /*
      Poly 3 4 5 6 7 8 9
      Vec. 6 5 4 3 2 1 0
      s = 7; d = 3;
      i = 10 -> push = i - (d + s) = 0
      i = 12 -> push = 12 - (3 + 7) = 2
    */
    int adds = i - (this->lsd + size);
    // Push Zero between the largest
    for (int i = 0 ; i < adds ; i++) {
      values.insert(values.begin(), Matrix(1, 1).t());
    }
    values.insert(values.begin(), c);

    // Clean all Zeros from Smallest & Largest
    clean();
  }
}

// Erase will just put a zero matrix to the degree, delete if it's largest / smallest
bool Polynomial::erase(int i)
{
  // Out of Bound Erase
  if (i < getSmallestDegree() || i > getLargestDegree()) {
    return false;
  }
  // In Range Erase
  else if (i > getSmallestDegree() && i < getLargestDegree()) {
    put(Matrix(1, 1).t(), i);
    return true;
  }

  auto zero = make_shared<Matrix>(1, 1)->t();

  // If this is the only coefficient
  // then delete the only one coefficient
  if (getSmallestDegree() == getLargestDegree()) {
    // All Coefficients are zero -> initial values[0] as zero
    values.clear();
    values.push_back(make_shared<Matrix>(1, 1)->t());
    this->lsd = 0;
    return true;
    return true;
  }

  // Delete smallest degree
  if (i == getSmallestDegree()) {
    values.pop_back();
    this->lsd = this->lsd + 1;
  }
  // Delete Largest Degree
  else {
    values.erase(values.begin());
  }

  // Clean all Zeros from Smallest & Largest
  clean();

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
    json element = json::object();
    auto matrix = this->get(i)->toJson();
    element["degree"] = i;
    element["matrix"] = matrix;
    output.push_back(element);
  }

  return output;
}

string Polynomial::toString()
{
  return this->toJson().dump();
}