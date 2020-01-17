#include "Random.hpp"

shared_ptr<Integer> Random::genInteger(const shared_ptr<Integer> &max, const vector<uint8_t> &seed)
{
  return genInteger(max->toBinary(), seed);
}

shared_ptr<Integer> Random::genInteger(vector<uint8_t> _max, const vector<uint8_t> &seed)
{
  string hex = Integer::createWithBinary(_max)->sub(Integer::ONE())->toHex();
  CryptoPP::Integer max(hex.c_str());

  if (seed.size() == 0)
  {
    // No Seed
    RandomGenerator prng;
    return make_shared<IntegerImpl>(CryptoPP::Integer(prng, CryptoPP::Integer::Zero(), max));
  }
  else
  {
    // With Seed
    CryptoPP::byte *s = (CryptoPP::byte *)seed.data();
    RandomGenerator rng(s, (size_t)seed.size());

    return make_shared<IntegerImpl>(CryptoPP::Integer(rng, CryptoPP::Integer::Zero(), max));
  }
}

shared_ptr<Integer> Random::genInteger(int byteLength, bool prime, const vector<uint8_t> &seed)
{
  CryptoPP::Integer x;
  int length = byteLength * 8;

  CryptoPP::AlgorithmParameters params;
  // Set Bit Length
  if (!prime)
    params = CryptoPP::MakeParameters("BitLength", length);
  // Set Prime
  else
    params = CryptoPP::MakeParameters("BitLength", length)("RandomNumberType", CryptoPP::Integer::PRIME);

  if (seed.size() == 0)
  {
    // No Seed
    RandomGenerator prng;
    x.GenerateRandom(prng, params);
    return make_shared<IntegerImpl>(x);
  }
  else
  {
    // With Seed
    CryptoPP::byte *s = (CryptoPP::byte *)seed.data();
    RandomGenerator rng(s, (size_t)seed.size());

    x.GenerateRandom(rng, params);
    return make_shared<IntegerImpl>(x);
  }
}

vector<uint8_t> Random::genBinary(int byteLength, const vector<uint8_t> &seed)
{
  auto retInt = Random::genInteger(byteLength, false, seed);
  auto retBin = retInt->toBinary();
  size_t len = retBin.size();
  if (len == byteLength)
    return retBin;

  vector<uint8_t> newBin(byteLength - len, 0x0);
  newBin.insert(newBin.end(), retBin.begin(), retBin.end());
  return newBin;
}

string Random::genHex(int byteLength, const vector<uint8_t> &seed)
{
  auto ret = Random::genBinary(byteLength, seed);
  return Utils::binaryToHex(ret);
}

vector<shared_ptr<Integer>> Random::getRandoms(size_t n, const shared_ptr<Integer> &modulus)
{
  vector<shared_ptr<Integer>> ret;
  for (size_t i = 0; i < n; i++)
  {
    auto rand = Random::genInteger(modulus, {});
    ret.push_back(rand);
  }
  return ret;
}
