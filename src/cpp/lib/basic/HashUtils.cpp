#include "HashUtils.hpp"

#include "../namespace.hpp"

#include "sha.h"
#include "hex.h"
#include "../utils/HexUtils.hpp"

vector<uint8_t> HashUtils::sha256(const vector<uint8_t> &message)
{
  size_t valueLength = message.size();
  CryptoPP::SHA256 hash;
  CryptoPP::byte digest[CryptoPP::SHA256::DIGESTSIZE];
  hash.Update(&message[0], valueLength);
  hash.Final(digest);
  vector<uint8_t> result(digest, digest + CryptoPP::SHA256::DIGESTSIZE);
  return result;
}
