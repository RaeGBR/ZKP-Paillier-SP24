#include "MathUtils.hpp"

shared_ptr<Integer> MathUtils::encodeComplement(shared_ptr<Integer> x, int bits)
{
  auto ZERO = Integer::pow2(bits - 1);
  return ZERO->add(x);
}

shared_ptr<Integer> MathUtils::decodeComplement(shared_ptr<Integer> x, int bits)
{
  auto ZERO = Integer::pow2(bits - 1);
  return x->sub(ZERO);
}

shared_ptr<Integer> MathUtils::decimalZero(unsigned long precision)
{
  string zeroStr = "1" + string(precision, '0');
  return make_shared<IntegerImpl>(zeroStr.c_str(), 10);
}

shared_ptr<Integer> MathUtils::encodeDecimal(string x, unsigned long precision, unsigned long scale)
{
  try
  {
    stod(x);
  }
  catch (string e)
  {
    throw invalid_argument("Invalid decimal number");
  }

  // Find the 'Dot' Position
  size_t pos = x.find(".");
  string integer = x, decimal;
  if (pos != string::npos)
  {
    // Get The Integer Part
    integer = x.substr(0, pos);

    // Get The Decimal Part
    decimal = x.substr(pos + 1, x.length());
  }

  if (scale > decimal.length())
  {
    // Add '0' in decimal
    decimal.append(string(scale - decimal.length(), '0'));
  }
  else
  {
    // Trim the number
    decimal = decimal.substr(0, scale);
  }

  // The Integer Result
  auto result = std::make_shared<IntegerImpl>((integer + decimal).c_str(), 10);
  return result->add(MathUtils::decimalZero(precision));
}

string MathUtils::decodeDecimal(shared_ptr<Integer> x, unsigned long precision, unsigned long scale)
{
  x = x.get()->sub(MathUtils::decimalZero(precision));

  string result = x.get()->toString();

  // insert "."
  if (scale > 0)
  {
    result.insert(result.length() - scale, 1, '.');
  }

  // insert padding "0" if -1 < x < 1
  if (result[0] == '.')
  {
    result.insert(0, 1, '0');
  }
  else if (result[0] == '-' && result[1] == '.')
  {
    result.insert(1, 1, '0');
  }

  return result;
}
