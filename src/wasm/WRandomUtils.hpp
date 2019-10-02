#pragma once

#include "RandomUtils.hpp"

using namespace cryptoplus;

#include <emscripten/bind.h>
EMSCRIPTEN_BINDINGS(RandomUtils)
{
  emscripten::class_<RandomUtils>("RandomUtils")
      .class_function("random", &RandomUtils::random)
      .class_function("randomWithSeed", &RandomUtils::randomWithSeed)
      .class_function("randomHex", &RandomUtils::randomHex)
      .class_function("randomHexWithSeed", &RandomUtils::randomHexWithSeed)
      .class_function("randomInt", &RandomUtils::randomInt)
      .class_function("randomIntWithSeed", &RandomUtils::randomIntWithSeed);
}
