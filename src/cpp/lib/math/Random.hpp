#pragma once

#include <string>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include "integer.h"
#include "osrng.h"
#include "algparam.h"

#include "Integer.hpp"

#include "./RandomGenerator.hpp"
#include "./IntegerImpl.hpp"

using namespace std;
using namespace cryptoplus;

namespace cryptoplus
{

class Random
{
public:
  // Random Integer with Max Integer (exclusive: 0 to max -1)
  static shared_ptr<Integer> genInteger(const shared_ptr<Integer> &max, const bool positiveOnly = false);

  // Random Integer with Max Integer (exclusive: 0 to max -1)
  static shared_ptr<Integer> genInteger(vector<uint8_t> max, const bool positiveOnly = false);

  // Random Integer
  static shared_ptr<Integer> genInteger(int byteLength, bool prime = false, const vector<uint8_t> &seed = vector<uint8_t>());

  // Random Hex
  static string genHex(int byteLength, const vector<uint8_t> &seed = vector<uint8_t>());

  // Random Binary
  static vector<uint8_t> genBinary(int byteLength, const vector<uint8_t> &seed = vector<uint8_t>());

  // Get a list of random numbers
  static vector<shared_ptr<Integer>> getRandoms(size_t n, const shared_ptr<Integer> &modulus, const bool positiveOnly = false);
};

} // namespace cryptoplus
