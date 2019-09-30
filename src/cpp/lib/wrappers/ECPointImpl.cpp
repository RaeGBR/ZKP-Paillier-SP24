#include "ECPointImpl.hpp"

using namespace std;
using namespace cryptoplus;

// Djinni Wrapper
shared_ptr<ECPoint> ECPoint::create(const shared_ptr<Integer> &x, const shared_ptr<Integer> &y)
{
  return make_shared<ECPointImpl>(x, y);
}

// Class Implement
ECPointImpl::ECPointImpl(const CryptoPP::ECPPoint &point)
{
  _point = make_shared<CryptoPP::ECPPoint>(point);
}

ECPointImpl::ECPointImpl(const shared_ptr<CryptoPP::ECPPoint> &point)
{
  _point = point;
}

ECPointImpl::ECPointImpl(const shared_ptr<Integer> &x, const shared_ptr<Integer> &y)
{
  setPoint(x, y);
}

ECPointImpl::ECPointImpl(const char *x, const char *y)
{
  string _x(x);
  string _y(y);
  setPoint(Integer::createWithString(_x), Integer::createWithString(_y));
}

shared_ptr<Integer> ECPointImpl::getX()
{
  return make_shared<IntegerImpl>(_point->x);
}

shared_ptr<Integer> ECPointImpl::getY()
{
  return make_shared<IntegerImpl>(_point->y);
}

void ECPointImpl::setX(const shared_ptr<Integer> &num)
{
  setPoint(num, getY());
}

void ECPointImpl::setY(const shared_ptr<Integer> &num)
{
  setPoint(getX(), num);
}

bool ECPointImpl::eq(const shared_ptr<ECPoint> &b)
{
  return getX()->eq(b->getX()) &&
         getY()->eq(b->getY());
}

shared_ptr<ECPoint> ECPointImpl::inv(const shared_ptr<EC> &curve)
{
  auto a = make_shared<ECPointImpl>(getPoint());
  return curve->inv(a);
}

shared_ptr<ECPoint> ECPointImpl::add(const shared_ptr<EC> &curve, const shared_ptr<ECPoint> &b)
{
  auto a = make_shared<ECPointImpl>(getPoint());
  return curve->add(a, b);
}

shared_ptr<ECPoint> ECPointImpl::sub(const shared_ptr<EC> &curve, const shared_ptr<ECPoint> &b)
{
  auto a = make_shared<ECPointImpl>(getPoint());
  return curve->sub(a, b);
}

shared_ptr<ECPoint> ECPointImpl::mul(const shared_ptr<EC> &curve, const shared_ptr<Integer> &b)
{
  auto a = make_shared<ECPointImpl>(getPoint());
  return curve->mul(b, a);
}

shared_ptr<CryptoPP::ECPPoint> ECPointImpl::getPoint()
{
  return _point;
}

void ECPointImpl::setPoint(shared_ptr<CryptoPP::ECPPoint> point)
{
  _point = point;
}

void ECPointImpl::setPoint(shared_ptr<Integer> x, shared_ptr<Integer> y)
{
  auto _x = dynamic_pointer_cast<IntegerImpl>(x);
  auto _y = dynamic_pointer_cast<IntegerImpl>(y);
  _point = make_shared<CryptoPP::ECPPoint>(*(_x->getValue()), *(_y->getValue()));
}
