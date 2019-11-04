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
  // Random Integer with Max Integer (inclusive)
  static shared_ptr<Integer> genInteger(vector<uint8_t> max, const vector<uint8_t> &seed = vector<uint8_t>());

  // Random Integer
  static shared_ptr<Integer> genInteger(int byteLength, bool prime = false, const vector<uint8_t> &seed = vector<uint8_t>());

  // Random Hex
  static string genHex(int byteLength, const vector<uint8_t> &seed = vector<uint8_t>());

  // Random Binary
  static vector<uint8_t> genBinary(int byteLength, const vector<uint8_t> &seed = vector<uint8_t>());

  // Get a list of random numbers
  static vector<shared_ptr<Integer>> getRandoms(size_t n, const shared_ptr<Integer> &modulus);
};

} // namespace cryptoplus
