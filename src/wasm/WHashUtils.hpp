#pragma once

#include "HashUtils.hpp"

using namespace cryptoplus;

#include <emscripten/bind.h>
EMSCRIPTEN_BINDINGS(HashUtils)
{
  emscripten::class_<HashUtils>("HashUtils")
      .class_function("sha256", &HashUtils::sha256);
}
