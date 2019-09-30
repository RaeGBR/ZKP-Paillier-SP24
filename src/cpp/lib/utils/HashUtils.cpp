#include "HashUtils.hpp"

using namespace cryptoplus;

SHA256 HashUtils::sha256;

string HashUtils::calcHash(string value, bool decodeInput, bool encodeOutput)
{
  string digest;

  CryptoPP::StringSource s(
      decodeInput ? HexUtils::decode(value) : value,
      true,
      new CryptoPP::HashFilter(sha256, HexUtils::getHexEncoder(digest)));

  return encodeOutput ? digest : HexUtils::decode(digest);
}

vector<uint8_t> HashUtils::calcHash(vector<uint8_t> value)
{
  size_t valueLength = value.size();
  uint8_t* v = &value[0];
  CryptoPP::SHA256 hash;
  CryptoPP::byte digest[CryptoPP::SHA256::DIGESTSIZE];
  hash.Update(v, valueLength);
  hash.Final(digest);
  vector<uint8_t> result(digest, digest+CryptoPP::SHA256::DIGESTSIZE);
  return result;
}