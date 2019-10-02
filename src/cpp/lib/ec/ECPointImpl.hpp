#pragma once

#include "../namespace.hpp"

#include <iostream>
#include "ecp.h"

#include "ECPoint.hpp"
#include "Integer.hpp"
#include "../math/IntegerImpl.hpp"
#include "ECCurve.hpp"

using namespace std;

namespace cryptoplus
{

/**
 * Elliptic Curve Point Class
 */
class ECPointImpl : public ECPoint
{
private:
  shared_ptr<CryptoPP::ECPPoint> _point;

public:
  // Constructor
  ECPointImpl(const CryptoPP::ECPPoint &point);
  ECPointImpl(const shared_ptr<CryptoPP::ECPPoint> &point);
  ECPointImpl(const shared_ptr<Integer> &x, const shared_ptr<Integer> &y);
  ECPointImpl(const char *x, const char *y);

  // Interface
  std::shared_ptr<Integer> getX();
  std::shared_ptr<Integer> getY();
  void setX(const std::shared_ptr<Integer> &num);
  void setY(const std::shared_ptr<Integer> &num);
  bool eq(const std::shared_ptr<ECPoint> &b);
  std::shared_ptr<ECPoint> inv(const std::shared_ptr<ECCurve> &curve);
  std::shared_ptr<ECPoint> add(const std::shared_ptr<ECCurve> &curve, const std::shared_ptr<ECPoint> &b);
  std::shared_ptr<ECPoint> sub(const std::shared_ptr<ECCurve> &curve, const std::shared_ptr<ECPoint> &b);
  std::shared_ptr<ECPoint> mul(const std::shared_ptr<ECCurve> &curve, const std::shared_ptr<Integer> &b);

  // Internal Function
  shared_ptr<CryptoPP::ECPPoint> getPoint();
  void setPoint(shared_ptr<CryptoPP::ECPPoint> point);
  void setPoint(shared_ptr<Integer> x, shared_ptr<Integer> y);

  friend ostream &operator<<(std::ostream &out, ECPoint &pt)
  {
    out << "(X: " << pt.getX()->toString() << ", Y: " << pt.getY()->toString() << ")";
    return out;
  }

  friend bool operator==(ECPoint &lhs, ECPoint &rhs)
  {
    return lhs.getX()->eq(rhs.getX()) &&
           lhs.getY()->eq(rhs.getY());
  }
};

} // namespace cryptoplus
