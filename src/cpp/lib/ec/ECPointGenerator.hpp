#pragma once

#include "../namespace.hpp"

#include <string>
#include <sstream>
#include "nbtheory.h"

#include "HashUtils.hpp"
#include "Utils.hpp"
#include "Integer.hpp"
#include "ECCurve.hpp"
#include "ECPoint.hpp"

using namespace std;
using namespace cryptoplus;

class ECPointGenerator
{
private:
  static shared_ptr<Integer> findY(const shared_ptr<ECCurve> &curve, shared_ptr<Integer> x);
  static shared_ptr<ECPoint> compute(const shared_ptr<ECCurve> &curve, shared_ptr<Integer> x);

public:
  static shared_ptr<ECPoint> generate(const shared_ptr<ECCurve> &curve, const binary_t &seed, bool isPositive);
};
