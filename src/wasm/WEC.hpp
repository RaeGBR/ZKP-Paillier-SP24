#pragma once

#include "ECCurve.hpp"

using namespace cryptoplus;

#include <emscripten/bind.h>
EMSCRIPTEN_BINDINGS(ECCurve)
{
  emscripten::class_<ECCurve>("ECCurve")
      .smart_ptr_constructor("ECCurve", &ECCurve::create)
      .class_function("create", &ECCurve::create)
      .class_function("SECP256K1", &ECCurve::SECP256K1)
      .class_function("getPublicElement", &ECCurve::getPublicElement)
      .class_function("getPrivateElement", &ECCurve::getPrivateElement)
      .function("getP", &ECCurve::getP)
      .function("getA", &ECCurve::getA)
      .function("getB", &ECCurve::getB)
      .function("getN", &ECCurve::getN)
      .function("getG", &ECCurve::getG)
      .function("add", &ECCurve::add)
      .function("mul", &ECCurve::mul)
      .function("multiply", &ECCurve::multiply)
      .function("verify", &ECCurve::verify)
      .function("encodePoint", &ECCurve::encodePoint)
      .function("decodePoint", &ECCurve::decodePoint);
}
