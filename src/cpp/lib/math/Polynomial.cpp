#include "./Polynomial.hpp"

Polynomial::Polynomial()
{
  this->lsd = 0; 
}

Polynomial::Polynomial(const shared_ptr<Polynomial> &poly)
{
  this->lsd = poly->lsd;
  for (size_t i = 0 ; i < poly->values.size() ; i++) {
    values[i] = poly->values[i];
  }
}

// D is the least significant degree
Polynomial::Polynomial(const vector<shared_ptr<Matrix>> ci, int d)
{
  this->lsd = d;
  values = ci;
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
}

