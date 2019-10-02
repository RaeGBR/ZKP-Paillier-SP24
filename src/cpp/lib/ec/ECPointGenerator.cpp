#include "ECPointGenerator.hpp"

shared_ptr<Integer> ECPointGenerator::findY(const shared_ptr<ECCurve> &curve, shared_ptr<Integer> x)
{
  try
  {
    // y^2 = x^3 + ax + b
    auto y2 = x->mul(x)->mul(x)->add(curve->getA()->mul(x))->add(curve->getB());
    return y2->modSqrt(curve->getP());
  }
  // Return Zero
  catch (char *str)
  {
    return Integer::ZERO();
  }
}

shared_ptr<ECPoint> ECPointGenerator::compute(const shared_ptr<ECCurve> &curve, shared_ptr<Integer> x)
{
  auto y = ECPointGenerator::findY(curve, x);
  bool valid = curve->verify(ECPoint::create(x, y));
  // Hash until find a point
  while (!valid || Integer::ZERO()->eq(y))
  {
    binary_t hash = HashUtils::sha256(x->toBinary());
    x = Integer::createWithBinary(hash);
    x = x->mod(curve->getP());
    y = ECPointGenerator::findY(curve, x);
    valid = curve->verify(ECPoint::create(x, y));
  }
  return ECPoint::create(x, y);
}

shared_ptr<ECPoint> ECPointGenerator::generate(const shared_ptr<ECCurve> &curve, const binary_t &seed, bool isPositive)
{
  binary_t hash = HashUtils::sha256(seed);
  auto x = Integer::createWithBinary(hash);
  x = x->mod(curve->getP());
  auto point = ECPointGenerator::compute(curve, x);

  // take negative root, Y = P - Y
  if (!isPositive)
  {
    point->setY(curve->getP()->sub(point->getY()));
  }

  return point;
}
