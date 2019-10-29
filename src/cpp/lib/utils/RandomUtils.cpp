#include "RandomUtils.hpp"

#include "../namespace.hpp"

#include "../math/RandomGenerator.hpp"
#include "../math/Random.hpp"

vector<uint8_t> RandomUtils::random(int32_t byteLength)
{
  return Random::genBinary(byteLength);
}

vector<uint8_t> RandomUtils::randomWithSeed(int32_t byteLength, const vector<uint8_t> &seed)
{
  return Random::genBinary(byteLength, seed);
}

string RandomUtils::randomHex(int32_t byteLength)
{
  return Random::genHex(byteLength);
}

string RandomUtils::randomHexWithSeed(int32_t byteLength, const vector<uint8_t> &seed)
{
  return Random::genHex(byteLength, seed);
}

shared_ptr<Integer> RandomUtils::randomInt(int32_t byteLength)
{
  return Random::genInteger(byteLength, false);
}

shared_ptr<Integer> RandomUtils::randomIntWithSeed(int32_t byteLength, const vector<uint8_t> &seed)
{
  return Random::genInteger(byteLength, false, seed);
}
