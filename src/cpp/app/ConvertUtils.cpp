#include "./ConvertUtils.hpp"

ZZ ConvertUtils::toZZ(const ZZ_p &input)
{
  return conv<ZZ>(input);
}

ZZ_p ConvertUtils::toZZ_p(const ZZ &input)
{
  return conv<ZZ_p>(input);
}

ZZ ConvertUtils::toZZ(const shared_ptr<Integer> &input)
{
  return conv<ZZ>(input->toString().c_str());
}

ZZ_p ConvertUtils::toZZ_p(const shared_ptr<Integer> &input)
{
  return conv<ZZ_p>(input->toString().c_str());
}

void ConvertUtils::toVecZZ(const vector<shared_ptr<Integer>> &input, Vec<ZZ> &output, size_t len)
{
  len = len == 0 ? input.size() : len;
  output.SetLength(len);

  for (size_t i = 0; i < len; i++)
  {
    output[i] = toZZ(input[i]);
  }
}

void ConvertUtils::toVecZZ_p(const vector<shared_ptr<Integer>> &input, Vec<ZZ_p> &output, size_t len)
{
  len = len == 0 ? input.size() : len;
  output.SetLength(len);

  for (size_t i = 0; i < len; i++)
  {
    output[i] = toZZ_p(input[i]);
  }
}

void ConvertUtils::toVecZZ(const Vec<ZZ_p> &input, Vec<ZZ> &output, size_t len)
{
  len = len == 0 ? input.length() : len;
  output.SetLength(len);

  for (size_t i = 0; i < len; i++)
  {
    output[i] = conv<ZZ>(input[i]);
  }
}

void ConvertUtils::toVecZZ_p(const Vec<ZZ> &input, Vec<ZZ_p> &output, size_t len)
{
  len = len == 0 ? input.length() : len;
  output.SetLength(len);

  for (size_t i = 0; i < len; i++)
  {
    output[i] = conv<ZZ_p>(input[i]);
  }
}

string ConvertUtils::toString(const ZZ &input)
{
  stringstream ss;
  ss << input;
  return ss.str();
}

string ConvertUtils::toString(const ZZ_p &input)
{
  stringstream ss;
  ss << input;
  return ss.str();
}

string ConvertUtils::toBinaryString(const ZZ &input)
{
  binary_t bin = toBinary(input);
  stringstream ss;
  for (int i = bin.size() - 1; i >= 0; i--)
  {
    ss << bitset<8>(bin[i]);
  }
  return ss.str();
}

string ConvertUtils::toBinaryString(const ZZ_p &input)
{
  return toBinaryString(toZZ(input));
}

void ConvertUtils::subVec(const Vec<ZZ_p> &input, Vec<ZZ_p> &output, size_t start, size_t end)
{
  end == 0 && (end = input.length());
  size_t n = end - start;
  output.SetLength(n);

  for (size_t i = 0; i < n; i++)
  {
    output[i] = input[start + i];
  }
}

ZZ ConvertUtils::toZZ(const binary_t &input)
{
  return ZZFromBytes(input.data(), input.size());
}

ZZ_p ConvertUtils::toZZ_p(const binary_t &input)
{
  return toZZ_p(toZZ(input));
}

void ConvertUtils::toVecZZ(const vector<binary_t> &input, Vec<ZZ> &output, size_t len)
{
  len = len == 0 ? input.size() : len;
  output.SetLength(len);

  for (size_t i = 0; i < len; i++)
  {
    output[i] = toZZ(input[i]);
  }
}

void ConvertUtils::toVecZZ_p(const vector<binary_t> &input, Vec<ZZ_p> &output, size_t len)
{
  Vec<ZZ> tmp;
  toVecZZ(input, tmp);
  toVecZZ_p(tmp, output);
}

unsigned char hexToChar(const char &c)
{
  const char zero = 48;
  const char nine = 57;
  const char A = 65;
  const char F = 70;
  const char a = 97;
  const char f = 102;

  if (c >= zero && c <= nine)
    return c - zero;

  if (c >= a && c <= f)
    return 10 + c - a;

  if (c >= A && c <= F)
    return 10 + c - A;

  return 0;
}

binary_t ConvertUtils::hexToBinary(const string &input)
{
  binary_t ret;
  unsigned char lower = 0;
  unsigned char higher = 0;
  auto size = input.length();
  for (size_t i = 0; i < size; i++)
  {
    auto c = input[size - i - 1];
    auto f = hexToChar(c);
    if (i % 2 == 0)
    {
      lower = f;
      continue;
    }

    higher = f << 4;
    ret.push_back(higher + lower);
  }

  return ret;
}

ZZ ConvertUtils::hexToZZ(const string &input)
{
  return toZZ(hexToBinary(input));
}
ZZ_p ConvertUtils::hexToZZ_p(const string &input)
{
  return toZZ_p(hexToZZ(input));
}

binary_t ConvertUtils::binaryStringToBinary(const string &input)
{
  binary_t output;
  for (int i = input.size(); i > 0; i -= 8)
  {
    int start = i - 8;
    size_t n = start < 0 ? 8 + start : 8;
    start = start < 0 ? 0 : start;
    string str = input.substr(start, n);
    uint8_t bit = bitset<8>(str).to_ulong();
    output.push_back(bit);
  }
  return output;
}

ZZ ConvertUtils::binaryStringToZZ(const string &input)
{
  return toZZ(binaryStringToBinary(input));
}

ZZ_p ConvertUtils::binaryStringToZZ_p(const string &input)
{
  return toZZ_p(binaryStringToZZ(input));
}

void ConvertUtils::fixBinary(binary_t &input, size_t len)
{
  input.resize(len, 0);
}

binary_t ConvertUtils::toBinary(const ZZ &input)
{
  auto size = NumBytes(input);
  binary_t output(size);
  BytesFromZZ(output.data(), input, size);
  return output;
}

binary_t ConvertUtils::toBinary(const ZZ_p &input)
{
  return toBinary(conv<ZZ>(input));
}

binary_t ConvertUtils::toBinary(const string &input)
{
  return binary_t(input.begin(), input.end());
}

string ConvertUtils::toString(const binary_t &input)
{
  return string(input.begin(), input.end());
}

binary_t ConvertUtils::concat(const binary_t &a, const binary_t &b)
{
  binary_t ret(a.begin(), a.end());
  ret.insert(ret.end(), b.begin(), b.end());
  return ret;
}

void ConvertUtils::append(binary_t &a, const binary_t &b)
{
  a.insert(a.end(), b.begin(), b.end());
}

binary_t ConvertUtils::concat(const binary_t &a, const string &b)
{
  return ConvertUtils::concat(a, ConvertUtils::toBinary(b));
}
binary_t ConvertUtils::concat(const string &a, const binary_t &b)
{
  return ConvertUtils::concat(ConvertUtils::toBinary(a), b);
}

binary_t ConvertUtils::padZero(const binary_t &data, const size_t length)
{
  binary_t ret(length, 0x00);
  return ConvertUtils::concat(ret, data);
}

bool ConvertUtils::eq(const binary_t &a, const binary_t &b)
{
  if (a.size() != b.size())
    return false;

  for (int i = 0; i < a.size(); i++)
  {
    if (a[i] != b[i])
      return false;
  }
  return true;
}
