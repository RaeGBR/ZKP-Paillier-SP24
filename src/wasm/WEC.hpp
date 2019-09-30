#pragma once

#include "EC.hpp"

using namespace cryptoplus;

#include <emscripten/bind.h>
EMSCRIPTEN_BINDINGS(EC)
{
  emscripten::class_<EC>("EC")
      .smart_ptr_constructor("EC", &EC::create)
      .class_function("create", &EC::create)
      .class_function("SECP256K1", &EC::SECP256K1)
      .class_function("getPublicElement", &EC::getPublicElement)
      .class_function("getPrivateElement", &EC::getPrivateElement)
      .function("getP", &EC::getP)
      .function("getA", &EC::getA)
      .function("getB", &EC::getB)
      .function("getN", &EC::getN)
      .function("getG", &EC::getG)
      .function("add", &EC::add)
      .function("mul", &EC::mul)
      .function("multiply", &EC::multiply)
      .function("verify", &EC::verify)
      .function("encodePoint", &EC::encodePoint)
      .function("decodePoint", &EC::decodePoint);
}
