#include "Random.hpp"
/*
shared_ptr<Integer> Random::genInteger(
    const shared_ptr<Integer> &max,
    const binary_t &seed,
    const bool positiveOnly)
{
  return genInteger(max->toBinary(), seed, positiveOnly);
}

shared_ptr<Integer> Random::genInteger(
    const binary_t &_max,
    const binary_t &seed,
    const bool positiveOnly)
{
  string hex = Integer::createWithBinary(_max)->sub(Integer::ONE())->toHex();
  CryptoPP::Integer max(hex.c_str());

  shared_ptr<IntegerImpl> ret;
  binary_t hash = seed;
  do
  {
    if (seed.size() == 0)
    {
      // No Seed
      RandomGenerator prng;
      ret = make_shared<IntegerImpl>(CryptoPP::Integer(prng, CryptoPP::Integer::Zero(), max));
    }
    else
    {
      // With Seed
      hash = HashUtils::sha256(hash);
      CryptoPP::byte *s = (CryptoPP::byte *)hash.data();
      RandomGenerator rng(s, (size_t)hash.size());
      ret = make_shared<IntegerImpl>(CryptoPP::Integer(rng, CryptoPP::Integer::Zero(), max));
    }
  } while (positiveOnly && ret->eq(Integer::ZERO()));

  return ret;
}
/*/

shared_ptr<Integer> Random::genInteger(
    const shared_ptr<Integer> &max,
    const binary_t &seed,
    const bool positiveOnly)
{
  shared_ptr<Integer> ret;
  size_t byteLength = max->toBinary().size();
  binary_t hash = seed;
  do
  {
    if (seed.size() == 0)
    {
      // No Seed
      ret = genInteger(byteLength, false, seed)->mod(max);
    }
    else
    {
      // With Seed
      hash = HashUtils::sha256(hash);
      ret = genInteger(byteLength, false, hash)->mod(max);
    }
  } while (positiveOnly && ret->eq(Integer::ZERO()));

  return ret;
}

shared_ptr<Integer> Random::genInteger(
    const binary_t &_max,
    const binary_t &seed,
    const bool positiveOnly)
{
  auto max = Integer::createWithBinary(_max);
  return genInteger(max, seed, positiveOnly);
}
//*/

shared_ptr<Integer> Random::genInteger(int byteLength, bool prime, const binary_t &seed)
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

binary_t Random::genBinary(int byteLength, const binary_t &seed)
{
  auto retInt = Random::genInteger(byteLength, false, seed);
  auto retBin = retInt->toBinary();
  size_t len = retBin.size();
  if (len == byteLength)
    return retBin;

  binary_t newBin(byteLength - len, 0x0);
  newBin.insert(newBin.end(), retBin.begin(), retBin.end());
  return newBin;
}

string Random::genHex(int byteLength, const binary_t &seed)
{
  auto ret = Random::genBinary(byteLength, seed);
  return Utils::binaryToHex(ret);
}

vector<shared_ptr<Integer>> Random::getRandoms(size_t n, const shared_ptr<Integer> &modulus, const bool positiveOnly)
{
  vector<shared_ptr<Integer>> ret;
  for (size_t i = 0; i < n; i++)
  {
    auto rand = Random::genInteger(modulus, {}, positiveOnly);
    ret.push_back(rand);
  }
  return ret;
}
