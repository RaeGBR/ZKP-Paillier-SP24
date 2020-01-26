#pragma once

#include "../namespace.hpp"

#include <stdexcept>
#include <vector>
#include <map>
#include <iterator>

#include "./IntegerImpl.hpp"
#include "./Matrix.hpp"

using namespace std;

namespace cryptoplus
{

class Polynomial
{
private:
  /**
    NOTES :
    1. Polynomial smallest index can be negative
    2. Need to marks the smallest index value (default is 0)
  */
  map<int, shared_ptr<Matrix>> values; // Polynomial Values
  int lsd = 0;                         // Least Significant Degree
  int msd = 0;                         // Most Significant Degree

public:
  Polynomial();                                            // All Zero Polynomial
  Polynomial(const vector<shared_ptr<Matrix>> &ci, int d); // Create the Polynomial

  // clone the Polynomial
  shared_ptr<Polynomial> clone();

  // transpose all coefficients
  shared_ptr<Polynomial> t();

  // get the smallest degree of x in the polynomial
  int getSmallestDegree();

  // get the largest degree of x in the polynomial
  int getLargestDegree();

  vector<int> getDegrees();

  // get term ci * x^i, return nullptr if not exists
  shared_ptr<Matrix> get(int i);

  // update the term ci * x^i
  void put(int i, const shared_ptr<Matrix> &c);

  // remove the term ci * x^i
  void erase(int i);

  // pure function, add 2 polynomial
  // eg.    a(x) = a1 * x + a2 * x^3
  //        b(x) = b1 * x + b2 * x^2
  // a(x) + b(x) = (a1+b1) * x + b2 * x^2 + a2 * x^3
  shared_ptr<Polynomial> add(const shared_ptr<Polynomial> &b, const shared_ptr<Integer> modulus = Integer::ZERO());

  // pure function, scalar multiplication for all matrix coefficients
  // eg. a(x) = a1 * x + a2 * x^3
  // b * a(x) = (b*a1) * x + (b*a2) * x^3
  shared_ptr<Polynomial> mul(const shared_ptr<Integer> &b, const shared_ptr<Integer> modulus = Integer::ZERO());

  // pure function, cross product for all matrix coefficients
  // eg. a(x) = a1 * x + a2 * x^3
  // b * a(x) = (b*a1) * x + (b*a2) * x^3
  shared_ptr<Polynomial> mul(const shared_ptr<Matrix> &b, const shared_ptr<Integer> modulus = Integer::ZERO());

  // pure function, dot product for all matrix coefficients
  // eg. a(x) = a1 * x + a2 * x^3
  // b . a(x) = (b.a1) * x + (b.a2) * x^3
  shared_ptr<Polynomial> dot(const shared_ptr<Matrix> &b, const shared_ptr<Integer> modulus = Integer::ZERO());

  // pure function, multiply 2 polynomial
  // eg.    a(x) = a1 * x + a2 * x^2
  //        b(x) = b1     + b2 * x
  // a(x) * b(x) = (a1*b1) * x + (a1*b2) * x^2 + (a2*b1) * x^2 + (a2*b2) * x^3
  // a(x) * b(x) = (a1*b1) * x + (a1*b2 + a2*b1) * x^2 + (a2*b2) * x^3
  shared_ptr<Polynomial> mul(const shared_ptr<Polynomial> &b, const shared_ptr<Integer> modulus = Integer::ZERO());

  // json toJson();
  // string toString();
};

} // namespace cryptoplus
