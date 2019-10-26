#pragma once

#include "../namespace.hpp"

#include <sstream>
#include <iostream>
#include "integer.h"
#include "nbtheory.h"

#include "Integer.hpp"
#include "Utils.hpp"

using namespace std;

namespace cryptoplus
{

/**
 * IntegerImpl Class
 */
class IntegerImpl : public Integer
{
private:
  shared_ptr<CryptoPP::Integer> _value;

public:
  // Constructor
  IntegerImpl();
  IntegerImpl(const char *value, const unsigned int radix = 16);
  IntegerImpl(const CryptoPP::Integer value);
  IntegerImpl(const shared_ptr<CryptoPP::Integer> value);
  IntegerImpl(const IntegerImpl &value);
  IntegerImpl(const vector<uint8_t> &value);
  IntegerImpl(int32_t value);

  // Interface
  bool eq(const std::shared_ptr<Integer> &b);
  bool gt(const std::shared_ptr<Integer> &b);
  bool gte(const std::shared_ptr<Integer> &b);
  bool lt(const std::shared_ptr<Integer> &b);
  bool lte(const std::shared_ptr<Integer> &b);

  std::shared_ptr<Integer> add(const std::shared_ptr<Integer> &b);
  std::shared_ptr<Integer> sub(const std::shared_ptr<Integer> &b);
  std::shared_ptr<Integer> mul(const std::shared_ptr<Integer> &b);
  std::shared_ptr<Integer> mod(const std::shared_ptr<Integer> &n);
  std::shared_ptr<Integer> div(const std::shared_ptr<Integer> &n);
  std::shared_ptr<Integer> inv(const std::shared_ptr<Integer> &n);
  std::shared_ptr<Integer> modMul(const std::shared_ptr<Integer> &b, const std::shared_ptr<Integer> &n);
  std::shared_ptr<Integer> modPow(const std::shared_ptr<Integer> &b, const std::shared_ptr<Integer> &n);
  std::shared_ptr<Integer> modSqrt(const std::shared_ptr<Integer> &n);
  int32_t compare(const std::shared_ptr<Integer> &n);
  std::shared_ptr<Integer> gcd(const std::shared_ptr<Integer> &n);

  string toString();
  string toHex();
  binary_t toBinary();
  binary_t toFixedBinary(const int32_t length);
  int32_t toNumber();
  string toBinaryString();

  // Internal Function
  shared_ptr<CryptoPP::Integer> getValue() const { return _value; }
};

} // namespace cryptoplus
