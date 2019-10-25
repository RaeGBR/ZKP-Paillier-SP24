#include "IntegerImpl.hpp"
#include <math.h>

using namespace std;
using namespace cryptoplus;

// Djinni Wrapper
shared_ptr<Integer> Integer::create(const string &num, int32_t radix)
{
  return make_shared<IntegerImpl>(num.c_str(), radix);
}

shared_ptr<Integer> Integer::createWithString(const string &num)
{
  return make_shared<IntegerImpl>(num.c_str());
}

shared_ptr<Integer> Integer::createWithBinaryString(const std::string &num)
{
  string input = num;
  return make_shared<IntegerImpl>(input.c_str(), 2);
}

shared_ptr<Integer> Integer::createWithBinary(const vector<uint8_t> &num)
{
  return make_shared<IntegerImpl>(num);
}

shared_ptr<Integer> Integer::createWithNumber(int32_t num)
{
  return make_shared<IntegerImpl>(num);
}

shared_ptr<Integer> Integer::ZERO()
{
  return make_shared<IntegerImpl>("0");
}

shared_ptr<Integer> Integer::ONE()
{
  return make_shared<IntegerImpl>("1");
}

shared_ptr<Integer> Integer::TWO()
{
  return make_shared<IntegerImpl>("2");
}

shared_ptr<Integer> Integer::pow2(int32_t n)
{
  return make_shared<IntegerImpl>(CryptoPP::Integer::Power2(n));
}

// Class Implement
IntegerImpl::IntegerImpl()
{
  _value = make_shared<CryptoPP::Integer>();
}

IntegerImpl::IntegerImpl(const char *value, const unsigned int radix)
{
  string v = value;
  if (v.length() > 2 && v[0] == '0' && v[1] == 'x')
  {
    v = v.substr(2);
  }

  char suffix = 'h';
  if (radix == 2)
    suffix = 'b';
  else if (radix == 8)
    suffix = 'o';
  else if (radix == 10)
    suffix = '.';

  if (v.length() > 0 && v[v.length() - 1] != suffix)
  {
    v += suffix;
  }
  _value = make_shared<CryptoPP::Integer>(v.c_str());
}

IntegerImpl::IntegerImpl(const CryptoPP::Integer value)
{
  _value = make_shared<CryptoPP::Integer>(value);
}

IntegerImpl::IntegerImpl(const shared_ptr<CryptoPP::Integer> value)
{
  _value = make_shared<CryptoPP::Integer>(*value);
}

IntegerImpl::IntegerImpl(const IntegerImpl &value)
{
  _value = make_shared<CryptoPP::Integer>(*value.getValue());
}

IntegerImpl::IntegerImpl(const vector<uint8_t> &value)
{
  auto n = Integer::createWithString(Utils::binaryToHex(value));
  auto _n = dynamic_pointer_cast<IntegerImpl>(n);
  _value = _n->getValue();
}

IntegerImpl::IntegerImpl(int32_t value)
{
  _value = make_shared<CryptoPP::Integer>(value);
}

bool IntegerImpl::eq(const shared_ptr<Integer> &b)
{
  auto _b = dynamic_pointer_cast<IntegerImpl>(b);
  return *_value == *_b->getValue();
}

bool IntegerImpl::gt(const shared_ptr<Integer> &b)
{
  auto _b = dynamic_pointer_cast<IntegerImpl>(b);
  return *_value > *_b->getValue();
}

bool IntegerImpl::gte(const shared_ptr<Integer> &b)
{
  auto _b = dynamic_pointer_cast<IntegerImpl>(b);
  return *_value >= *_b->getValue();
}

bool IntegerImpl::lt(const shared_ptr<Integer> &b)
{
  auto _b = dynamic_pointer_cast<IntegerImpl>(b);
  return *_value < *_b->getValue();
}

bool IntegerImpl::lte(const shared_ptr<Integer> &b)
{
  auto _b = dynamic_pointer_cast<IntegerImpl>(b);
  return *_value <= *_b->getValue();
}

shared_ptr<Integer> IntegerImpl::add(const shared_ptr<Integer> &b)
{
  auto _b = dynamic_pointer_cast<IntegerImpl>(b);
  return make_shared<IntegerImpl>((*_value) + (*_b->getValue()));
}

shared_ptr<Integer> IntegerImpl::sub(const shared_ptr<Integer> &b)
{
  auto _b = dynamic_pointer_cast<IntegerImpl>(b);
  return make_shared<IntegerImpl>((*_value) - (*_b->getValue()));
}

shared_ptr<Integer> IntegerImpl::mul(const shared_ptr<Integer> &b)
{
  auto _b = dynamic_pointer_cast<IntegerImpl>(b);
  return make_shared<IntegerImpl>((*_value) * (*_b->getValue()));
}

shared_ptr<Integer> IntegerImpl::mod(const shared_ptr<Integer> &b)
{
  auto _b = dynamic_pointer_cast<IntegerImpl>(b);
  return make_shared<IntegerImpl>((*_value) % (*_b->getValue()));
}

shared_ptr<Integer> IntegerImpl::inv(const shared_ptr<Integer> &n)
{
  auto _n = dynamic_pointer_cast<IntegerImpl>(n);
  return make_shared<IntegerImpl>(_value->InverseMod(*(_n->getValue())));
}

shared_ptr<Integer> IntegerImpl::modMul(const shared_ptr<Integer> &b, const shared_ptr<Integer> &n)
{
  auto _b = dynamic_pointer_cast<IntegerImpl>(b);
  auto _n = dynamic_pointer_cast<IntegerImpl>(n);
  return make_shared<IntegerImpl>(a_times_b_mod_c(*_value, *(_b->getValue()), *(_n->getValue())));
}

shared_ptr<Integer> IntegerImpl::modPow(const shared_ptr<Integer> &b, const shared_ptr<Integer> &n)
{
  auto _b = dynamic_pointer_cast<IntegerImpl>(b);
  auto _n = dynamic_pointer_cast<IntegerImpl>(n);
  return make_shared<IntegerImpl>(a_exp_b_mod_c(*_value, *(_b->getValue()), *(_n->getValue())));
}

shared_ptr<Integer> IntegerImpl::modSqrt(const shared_ptr<Integer> &n)
{
  auto _n = dynamic_pointer_cast<IntegerImpl>(n);
  return make_shared<IntegerImpl>(CryptoPP::ModularSquareRoot((*_value), (*_n->getValue())));
}

int32_t IntegerImpl::compare(const shared_ptr<Integer> &n)
{
  auto _n = dynamic_pointer_cast<IntegerImpl>(n);
  return _value->Compare(*(_n->getValue()));
}

shared_ptr<Integer> IntegerImpl::gcd(const shared_ptr<Integer> &n)
{
  auto _n = dynamic_pointer_cast<IntegerImpl>(n);
  return make_shared<IntegerImpl>(CryptoPP::Integer::Gcd((*_value), (*_n->getValue())));
}

string IntegerImpl::toString()
{
  ostringstream out;
  out << *_value;
  string ret = out.str();
  return ret.substr(0, ret.length() - 1);
}

string IntegerImpl::toHex()
{
  ostringstream out;
  out << uppercase << hex << *_value;
  string ret = out.str();
  ret = ret.substr(0, ret.length() - 1);
  if (ret[0] != '-' && ret.length() % 2 == 1)
  {
    ret = "0" + ret;
  }
  return ret;
}

vector<uint8_t> IntegerImpl::toBinary()
{
  return Utils::hexToBinary(toHex());
}

string IntegerImpl::toBinaryString()
{
  string out;
  auto hex = toBinary();
  for (int i = 0; i < hex.size(); i++)
  {
    string byte = "";
    uint8_t x = hex[i];
    for (int j = 0; j < 8; j++)
    {
      string bit = (x & 1) == 1 ? "1" : "0";
      byte = bit + byte;
      x = x >> 1;
    }
    out.append(byte);
  }
  return out;
}

int32_t IntegerImpl::toNumber()
{
  return _value->ConvertToLong();
}
