#include "HexUtils.hpp"

string HexUtils::encode(const string &input)
{
  string output;
  CryptoPP::StringSource ss(input, true, getHexEncoder(output));
  return output;
}

string HexUtils::decode(const string &input)
{
  string output;
  CryptoPP::StringSource ss(input, true, getHexDecoder(output));
  return output;
}

CryptoPP::HexEncoder *HexUtils::getHexEncoder(string &output)
{
  return new CryptoPP::HexEncoder(new CryptoPP::StringSink(output));
}

CryptoPP::HexDecoder *HexUtils::getHexDecoder(string &output)
{
  return new CryptoPP::HexDecoder(new CryptoPP::StringSink(output));
}

CryptoPP::StringSink *HexUtils::getStringSink(string &output)
{
  return new CryptoPP::StringSink(output);
}

string HexUtils::binaryToHex(const binary_t &bytes)
{
  binary_t binary = bytes;
  CryptoPP::byte *b = &binary[0];
  string encoded;
  CryptoPP::StringSource ss(b, bytes.size(), true, new CryptoPP::HexEncoder(new CryptoPP::StringSink(encoded)));
  return encoded;
}

string HexUtils::binaryToString(const binary_t &bytes)
{
  return string(bytes.begin(), bytes.end());
}

json HexUtils::binaryToJson(const binary_t &binary)
{
  return HexUtils::binaryToHex(binary);
}

json HexUtils::binaryToJson(const vector<binary_t> &binary)
{
  json ret = json::array();
  for (int i = 0; i < binary.size(); i++)
  {
    ret[i] = HexUtils::binaryToJson(binary[i]);
  }
  return ret;
}

binary_t HexUtils::stringToBinary(const string &plainText)
{
  return binary_t(plainText.begin(), plainText.end());
}

binary_t HexUtils::concat(const binary_t &a, const binary_t &b)
{
  binary_t ret(a.begin(), a.end());
  ret.insert(ret.end(), b.begin(), b.end());
  return ret;
}

void HexUtils::append(binary_t &a, const binary_t &b)
{
  a.insert(a.end(), b.begin(), b.end());
}

binary_t HexUtils::concat(const binary_t &a, const string &b)
{
  return HexUtils::concat(a, HexUtils::stringToBinary(b));
}
binary_t HexUtils::concat(const string &a, const binary_t &b)
{
  return HexUtils::concat(HexUtils::stringToBinary(a), b);
}

binary_t HexUtils::padZero(const binary_t &data, const size_t length)
{
  binary_t ret(length, 0x00);
  return HexUtils::concat(ret, data);
}

bool HexUtils::eq(const binary_t &a, const binary_t &b)
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
